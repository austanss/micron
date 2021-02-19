//
// Created by rizet on 1/26/21.
//
#include <stddef.h>
#include <stdint.h>
#include "kernel/kconfigf.hxx"
#include "kernel/memory.hxx"
#include "kernel/boot.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include "kernel/bitmap.hxx"

//////////////////////////////////////////
// variables
//////////////////////////////////////////

size_t memory::free_memory_size;
size_t memory::used_memory_size;
size_t memory::total_memory_size;
size_t memory::reserved_memory_size;
util::bitmap page_bitmap_map;
char* kernel_heap_allocator_ptr;
char* user_heap_allocator_ptr;
bool initialized = false;
memory::allocation::memory_info* mem_info;
memory::paging::page_table* memory::paging::pml_4;

bool allocator_on = false;

char *memory_types(uint16_t type)
{
    switch (type)
    {
        case 1: return "Conventional memory";
        case 2: return "Reserved memory";
        case 3: return "Reclaimable ACPI memory";
        case 4: return "ACPI NVS memory";
        case 5: return "Faulty memory";
        case 10: return "Kernel memory";
        case 4096: return "Reclaimable bootloader memory";
    }
}

//////////////////////////////////////////
// malloc
//////////////////////////////////////////

/*private*/ void init_bitmap(size_t bitmap_size, void* address)
{
	page_bitmap_map.size = bitmap_size;
    page_bitmap_map.buffer = (uint8_t*)address;
    for (size_t i = 0; i < bitmap_size; i++)
        *(uint8_t*)(page_bitmap_map.buffer + i) = 0;
} 

uint64_t memory::allocation::get_total_memory_size(stivale_memory_map* memory_map, uint64_t map_size, uint64_t desc_size)
{
    static uint64_t memory_size_bytes = 0;
    if (memory_size_bytes > 0) return memory_size_bytes;

    uint64_t map_entries = map_size / desc_size;
    uint64_t memory_size_pages = 0;

    for (uint64_t i = 0; i < map_entries; i++) {

        stivale_mmap_entry* desc = (stivale_mmap_entry *)((uint64_t)memory_map->memory_map_addr + (i * desc_size));
        
        if (desc->base < 0x100000)
            continue;

        memory_size_pages += (desc->length  / 0x1000);
    }

    memory_size_bytes = memory_size_pages * 0x1000;
    memory_size_bytes += 0x100000;

    memory_size_bytes /= 0x100000; //////////
    memory_size_bytes *= 0x100000; //// account for inaccuracies

    return memory_size_bytes;
}

extern "C" void reserve_pages(void* address, uint64_t page_count);

void memory::allocation::map_memory(stivale_memory_map* memory_map, uint64_t map_size, uint64_t desc_size)
{
    if (initialized) return;

    initialized = true;

    uint64_t map_entries = map_size / desc_size;

    void* largest_free_memory_segment = NULL;
    size_t largest_free_memory_segment_size = 0;

    for (uint64_t i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((uint64_t)memory_map->memory_map_addr + (i * desc_size));
        if (desc->type == 1 || desc->type == 3 || desc->type == 0x1000 || desc->type == 0x1001) { // usable memory
            if ((desc->length / 0x1000) * 4096 > largest_free_memory_segment_size)
            {
                largest_free_memory_segment = (void *)desc->base;
                largest_free_memory_segment_size = (desc->length  / 0x1000) * 4096;
            }
        }
    }

    uint64_t memory_size = get_total_memory_size(memory_map, map_size, desc_size);
    free_memory_size = memory_size;
    uint64_t bitmap_size = memory_size / 4096 / 8 + 1;     

    io::serial::serial_msg("Memory size (MiB): ");
    io::serial::serial_msg(util::itoa(memory_size / 0x100000, 10));
    io::serial::serial_msg("\nBitmap size (B): ");
    io::serial::serial_msg(util::itoa(bitmap_size, 10));
    io::serial::serial_msg("\n");

    init_bitmap(bitmap_size, largest_free_memory_segment);

    memory::paging::allocation::lock_pages(page_bitmap_map.buffer, page_bitmap_map.size / 4096 + 1);

    for (uint64_t i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((uint64_t)memory_map->memory_map_addr + (i * desc_size));
        io::serial::serial_msg("@ 0x");
        io::serial::serial_msg(util::itoa(desc->base, 16));
        io::serial::serial_msg(": ");
        io::serial::serial_msg(memory_types(desc->type));
        io::serial::serial_msg(", for ");
        io::serial::serial_msg(util::itoa((desc->length  / 0x1000), 10));
        io::serial::serial_msg(" pages\n");
        if (desc->type != 1) // not conventional memory
            reserve_pages((void *)desc->base, (desc->length  / 0x1000));
    }
    
    reserve_pages((void *)0x0, 0x100000 / 0x1000);
    reserve_pages((void *)&sys::config::_kernel_start, sys::config::_kernel_pages);
}

void* heap_start;
void* heap_end;

void memory::allocation::initialize_heap(void* heap_address, size_t heap_length) {
    heap_start = heap_address;
    heap_end = (void*)((size_t)heap_start + heap_length);
    heap_segment_header* start_segment = (heap_segment_header*)heap_address;
    start_segment->length = heap_length - sizeof(heap_segment_header);
    start_segment->last = NULL;
    start_segment->next = NULL;
    start_segment->free = true;
}

void* memory::allocation::malloc(uint64_t size) {
    if (size == 0) return nullptr;
    if (size % 0x10 != 0) size = size % 0x10 + 0x10; //Make sure to round up to 0x10

    heap_segment_header* current_segment = (heap_segment_header*)heap_start;
    while(true) {
        if (current_segment->free) {
            
            if (current_segment->length > size) {
                current_segment->split(size);
                current_segment->free = false;
                return current_segment;
            }
            if (current_segment->length == size ) {
                current_segment->free = false;
                return current_segment;
            }
        }
        if (current_segment->next == NULL) break;
        current_segment = current_segment->next;
    }
    return nullptr;
}

void memory::allocation::free(void* address) {
    heap_segment_header* current_segment = (heap_segment_header*)address;
    current_segment->free = true;
    if (current_segment->next != NULL && current_segment->next->free)current_segment->combine_forward();
    if (current_segment->last != NULL && current_segment->last->free)current_segment->combine_backward();
}

memory::allocation::heap_segment_header* memory::allocation::heap_segment_header::split(size_t split_length) {
    if (split_length < 0x10) return NULL;
    size_t splitSegLength = length - split_length - (sizeof(heap_segment_header) * 2);
    if (splitSegLength< 0x10) return NULL;
    heap_segment_header* new_split_header = (heap_segment_header*) ((size_t)this + split_length + sizeof(heap_segment_header));
    next->last = new_split_header; // Set the next segment's last segment to our new segment
    new_split_header->next = next; // Set the new segment's next segment to our original next segment
    next = new_split_header; // Set our next segment to our new segment
    new_split_header->last = this; // Set our new segment's last segment to the currect segment
    new_split_header->length = splitSegLength;
    new_split_header->free = free; // Make sure the new segment is as free as the original
    length = split_length;
    return new_split_header;
}

void memory::allocation::heap_segment_header::combine_forward() {
    if (next == NULL) return;
    if (!next->free) return;
    
    if (next->next != NULL) {
        next->next->last = this;
    }
    length = length + next->length + sizeof(heap_segment_header);

}

void memory::allocation::heap_segment_header::combine_backward() {
    if (last != NULL) last->combine_forward();
}

//////////////////////////////////////////
// paging
//////////////////////////////////////////
void unreserve_page(void* address) {

    uint64_t index = (uint64_t)address / 4096;

    if (page_bitmap_map[index] == false) 
		return;

    page_bitmap_map.set(index, false);
    memory::free_memory_size += 4096;
    memory::reserved_memory_size -= 4096;
}

void unreserve_pages(void* address, uint64_t pageCount) {

    for (size_t t = 0; t < pageCount; t++)
        unreserve_page((void*)((uint64_t)address + (t * 4096)));
}

extern "C" void reserve_page(void* address) 
{
    uint64_t index = (uint64_t)address / 4096;
/*
    io::serial::serial_msg("reserving page @ 0x");
    io::serial::serial_msg(util::itoa((long)address, 16));
    io::serial::serial_msg(" from page_bitmap_map[");
    io::serial::serial_msg(util::itoa(index, 10));
    io::serial::serial_msg("] (bitmap max index is ");
    io::serial::serial_msg(util::itoa(page_bitmap_map.size * 8 - 1, 10));
    io::serial::serial_msg(")...\n");
*/
    if (page_bitmap_map[index] == true) 
		return;

    page_bitmap_map.set(index, true);
    memory::free_memory_size -= 4096;
    memory::reserved_memory_size += 4096;
}

extern "C" void reserve_pages(void* address, uint64_t pageCount) {

    for (size_t t = 0; t < pageCount; t++)
        reserve_page((void*)((uint64_t)address + (t * 4096)));
}

extern "C" void* memory::paging::allocation::request_page() {

    for (uint64_t index = 0; index < page_bitmap_map.size * 8; index++) {

        if (page_bitmap_map[index] == true) 
			continue;
/*
        io::serial::serial_msg("requested page, located @paddr-0x");
        io::serial::serial_msg(util::itoa(index * 4096, 16));
        io::serial::serial_msg("\n");
*/
        memory::paging::allocation::lock_page((void*)(index * 4096));

        return (void*)(index * 4096);
    }

    return nullptr;
}

extern "C" void* memory::paging::allocation::request_pages(uint64_t page_count)
{
    if (page_count <= 1)
        return nullptr;

    void* start_ptr = request_page();
    page_count--;

    map_memory(start_ptr, start_ptr);


    for (uint64_t i = 1; i <= page_count; i++)
    {
        void* page_ptr = request_page();
        map_memory((void *)((uint64_t)start_ptr + (i * 0x1000)), page_ptr);
    }

    return start_ptr;
}

extern "C" void memory::paging::allocation::free_page(void* address) {

    uint64_t index = (uint64_t)address / 4096;
    if (page_bitmap_map[index] == false) return;
    page_bitmap_map.set(index, false);
    free_memory_size += 4096;
    used_memory_size -= 4096;
}

extern "C" void memory::paging::allocation::free_pages(void* address, uint64_t page_count) {
 
    for (size_t t = 0; t < page_count; t++)
        memory::paging::allocation::free_page((void*)((uint64_t)address + (t * 4096)));
}

extern "C" void memory::paging::allocation::lock_page(void* address) {
    uint64_t index = (uint64_t)address / 4096;
    if (page_bitmap_map[index] == true) return;
    page_bitmap_map.set(index, true);
    free_memory_size -= 4096;
    used_memory_size += 4096;
}

extern "C" void memory::paging::allocation::lock_pages(void* address, uint64_t page_count) {

    for (size_t t = 0; t < page_count; t++)
        memory::paging::allocation::lock_page((void*)((uint64_t)address + (t * 4096)));
}

memory::paging::page_map_indexer::page_map_indexer(uint64_t virtual_address) {

    virtual_address >>= 12;
    p_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pt_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pd_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pdp_i = virtual_address & 0x1ff;
}

void memory::paging::map_memory(void *virtual_memory, void *physical_memory)
{
    page_map_indexer indexer = page_map_indexer((uint64_t)virtual_memory);
    page_directory_entry pde;

    pde = pml_4->entries[indexer.pdp_i];
    page_table* pdp;
    if (!pde.get_flag(pt_flag::present)) {
    
        pdp = (page_table*)allocation::request_page();
        memory::operations::memset(pdp, 0, 0x1000);
        pde.set_address((uint64_t)pdp >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pml_4->entries[indexer.pdp_i] = pde;
        memory::paging::map_memory((void *)pdp, (void *)pdp);
    }
    else
        pdp = (page_table*)((uint64_t)pde.get_address() << 12);
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;
    if (!pde.get_flag(pt_flag::present)) {

        pd = (page_table*)allocation::request_page();
        memory::operations::memset(pd, 0, 0x1000);
        pde.set_address((uint64_t)pd >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pdp->entries[indexer.pd_i] = pde;
        memory::paging::map_memory((void *)pd, (void *)pd);
    }
    else
        pd = (page_table*)((uint64_t)pde.get_address() << 12);

    pde = pd->entries[indexer.pt_i];
    page_table* pt;
    if (!pde.get_flag(pt_flag::present)) {

        pt = (page_table*)allocation::request_page();
        memory::operations::memset(pt, 0, 0x1000);
        pde.set_address((uint64_t)pt >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pd->entries[indexer.pt_i] = pde;
        memory::paging::map_memory((void *)pt, (void *)pt);
    }
    else
        pt = (page_table*)((uint64_t)pde.get_address() << 12);


    pde = pt->entries[indexer.p_i];
    pde.set_address((uint64_t)physical_memory >> 12);
    pde.set_flag(pt_flag::present, true);
    pde.set_flag(pt_flag::read_write, true);
    pt->entries[indexer.p_i] = pde;
}

extern "C" void memory::paging::donate_to_userspace(void* virtual_address)
{
    page_map_indexer indexer = page_map_indexer((uint64_t)virtual_address);
    page_directory_entry pde;
    pde = pml_4->entries[indexer.pdp_i];
    pml_4->entries[indexer.pdp_i].set_flag(pt_flag::user_super, 1);
    page_table* pdp = (page_table*)((uint64_t)pde.get_address() << 12);
    pdp->entries[indexer.pd_i].set_flag(pt_flag::user_super, 1);
    pde = pdp->entries[indexer.pd_i];
    page_table* pd = (page_table*)((uint64_t)pde.get_address() << 12);
    pd->entries[indexer.pt_i].set_flag(pt_flag::user_super, 1);
    pde = pd->entries[indexer.pt_i];
    page_table* pt = (page_table*)((uint64_t)pde.get_address() << 12);
    pt->entries[indexer.p_i].set_flag(pt_flag::user_super, 1);
}

void memory::paging::page_directory_entry::set_flag(pt_flag flag, bool enabled) {

    uint64_t bit_selector = (uint64_t)1 << flag;
    value &= ~bit_selector;
    if (enabled)
        value |= bit_selector;
}

bool memory::paging::page_directory_entry::get_flag(pt_flag flag) {

    uint64_t bit_selector = (uint64_t)1 << flag;
    return (value & bit_selector) > 0 ? true : false;
}

uint64_t memory::paging::page_directory_entry::get_address() {

    return (value & 0x000ffffffffff000) >> 12;
}

void memory::paging::page_directory_entry::set_address(uint64_t address) {

    address &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (address << 12);
}

//////////////////////////////////////////
// memops
//////////////////////////////////////////

void *memory::operations::memcpy(void *__restrict dst, const void *__restrict src, size_t count)
{
	asm volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dst) :: "memory");
	return dst;
}

void* memory::operations::memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}