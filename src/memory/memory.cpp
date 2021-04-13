//
// Created by rizet on 1/26/21.
//
#include <stddef.h>
#include "types.h"
#include "config/kconf.h"
#include "memory/memory.h"
#include "start/boot.h"
#include "devio/io.h"
#include "util/kutil.h"
#include "util/bitmap.h"

//////////////////////////////////////////
// variables
//////////////////////////////////////////

uint memory::free_memory_size;
uint memory::used_memory_size;
uint memory::total_memory_size;
uint memory::reserved_memory_size;
util::bitmap page_bitmap_map;
bool initialized = false;
memory::allocation::memory_info* mem_info;
memory::paging::page_table* memory::paging::pml_4;

bool allocator_on = false;

const char *memory_types(uint16 type)
{
    switch (type)
    {
        case 1: return (const char *)"Conventional memory";
        case 2: return (const char *)"Reserved memory";
        case 3: return (const char *)"Reclaimable ACPI memory";
        case 4: return (const char *)"ACPI NVS memory";
        case 5: return (const char *)"Faulty memory";
        case 10: return (const char *)"Kernel memory";
        case 4096: return (const char *)"Reclaimable bootloader memory";
        default: return (const char *)"Unidentified memory";
    }
    return nullptr;
}

//////////////////////////////////////////
// malloc
//////////////////////////////////////////

/*private*/ void init_bitmap(uint64 bitmap_size, void* addr)
{
	page_bitmap_map.size = bitmap_size;
    page_bitmap_map.buffer = (byte*)addr;
    for (uint64 i = 0; i < bitmap_size; i++)
        *(byte *)(page_bitmap_map.buffer + i) = 0;
} 

uint memory::allocation::get_total_memory_size(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size) 
{
    static uint64 memory_size_bytes = 0;
    if (memory_size_bytes > 0) return memory_size_bytes;

    uint64 map_entries = map_size / desc_size;

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        
        if (desc->type == 1)
            memory_size_bytes = desc->base + desc->length;
    }

    memory_size_bytes /= 0x100000; //////////
    memory_size_bytes *= 0x100000; //// account for inaccuracies

    return memory_size_bytes;
}

extern "C" void reserve_pages(void* vaddress, uint64 page_count);

void memory::allocation::map_memory(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size)
{
    if (initialized) return;

    initialized = true;

    uint map_entries = map_size / desc_size;

    void* largest_free_memory_segment = NULL;
    uint largest_free_memory_segment_size = 0;

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        if (desc->type == 1) { // usable memory
            if ((desc->length / 0x1000) * 4096 > largest_free_memory_segment_size)
            {
                largest_free_memory_segment = (void *)desc->base;
                largest_free_memory_segment_size = (desc->length  / 0x1000) * 4096;
            }
        }
        
        io::serial::serial_msg("@ 0x");
        io::serial::serial_msg(util::itoa(desc->base, 16));
        io::serial::serial_msg(": ");
        io::serial::serial_msg(memory_types(desc->type));
        io::serial::serial_msg(", for ");
        io::serial::serial_msg(util::itoa((desc->length  / 0x1000), 10));
        io::serial::serial_msg(" pages\n");
    }

    uint memory_size = get_total_memory_size(memory_map, map_size, desc_size);
    total_memory_size = memory_size;
    free_memory_size = memory_size;
    uint bitmap_size = memory_size / 4096 / 8 + 1;     

    io::serial::serial_msg("Memory size (MiB): ");
    io::serial::serial_msg(util::itoa(memory_size / 0x100000, 10));
    io::serial::serial_msg("\nBitmap size (B): ");
    io::serial::serial_msg(util::itoa(bitmap_size, 10));
    io::serial::serial_msg("\n");

    init_bitmap(bitmap_size, largest_free_memory_segment);

    for (uint i = 0; i < map_entries; i++) {
        stivale_mmap_entry* desc = (stivale_mmap_entry *)((address)memory_map->memory_map_addr + (i * desc_size));
        if (desc->type != 1 && desc->base < memory_size) // not conventional memory
            reserve_pages((void *)desc->base, (desc->length  / 0x1000));
    }

    memory::paging::allocation::lock_pages(page_bitmap_map.buffer, page_bitmap_map.size / 4096 + 1);
    
    reserve_pages((void *)0x0, 0x100000 / 0x1000);
    reserve_pages((void *)&sys::config::__kernel_start, sys::config::__kernel_pages);
}

void* heap_start;
void* heap_end;

void memory::allocation::initialize_heap(void* heap_address, uint64 heap_length) {
    heap_start = heap_address;
    heap_end = (void*)((uint64)heap_start + heap_length);
    heap_segment_header* start_segment = (heap_segment_header*)heap_address;
    start_segment->length = heap_length - sizeof(heap_segment_header);
    start_segment->last = NULL;
    start_segment->next = NULL;
    start_segment->free = true;
}

void* memory::allocation::malloc(uint64 size) {
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

void memory::allocation::free(void* addr) {
    heap_segment_header* current_segment = (heap_segment_header*)addr;
    current_segment->free = true;
    if (current_segment->next != NULL && current_segment->next->free)current_segment->combine_forward();
    if (current_segment->last != NULL && current_segment->last->free)current_segment->combine_backward();
}

memory::allocation::heap_segment_header* memory::allocation::heap_segment_header::split(uint64 split_length) {
    if (split_length < 0x10) return NULL;
    uint64 splitSegLength = length - split_length - (sizeof(heap_segment_header) * 2);
    if (splitSegLength< 0x10) return NULL;
    heap_segment_header* new_split_header = (heap_segment_header*) ((uint64)this + split_length + sizeof(heap_segment_header));
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
    
    if (next->next != NULL)
        next->next->last = this;

    length = length + next->length + sizeof(heap_segment_header);

    next = next->next; 
}

void memory::allocation::heap_segment_header::combine_backward() {
    if (last != NULL) last->combine_forward();
}

//////////////////////////////////////////
// paging
//////////////////////////////////////////
void unreserve_page(void* vaddress) {

    uint64 index = (uint64)vaddress / 4096;

    if (page_bitmap_map[index] == false) 
		return;

    page_bitmap_map.set(index, false);
    memory::free_memory_size += 4096;
    memory::reserved_memory_size -= 4096;
}

void unreserve_pages(void* vaddress, uint64 pageCount) {

    for (uint t = 0; t < pageCount; t++)
        unreserve_page((void*)((uint64)vaddress + (t * 4096)));
}

extern "C" void reserve_page(void* vaddress) 
{
    uint64 index = (uint64)vaddress / 4096;

    if (page_bitmap_map[index] == true) 
		return;

    page_bitmap_map.set(index, true);
    memory::free_memory_size -= 4096;
    memory::reserved_memory_size += 4096;
}

extern "C" void reserve_pages(void* vaddress, uint64 pageCount) {

    for (uint t = 0; t < pageCount; t++)
        reserve_page((void*)((address)vaddress + (t * 4096)));
}

extern "C" void* memory::paging::allocation::request_page() {

    for (uint index = 0; index < page_bitmap_map.size * 8; index++) {

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

extern "C" void* memory::paging::allocation::request_pages(uint64 page_count)
{
    if (page_count <= 1)
        return nullptr;

    void* start_ptr = request_page();
    page_count--;

    map_memory(start_ptr, start_ptr, false);

    for (uint i = 1; i <= page_count; i++)
    {
        void* page_ptr = request_page();
        map_memory((void *)((uint64)start_ptr + (i * 0x1000)), page_ptr, false);
    }

    return start_ptr;
}

extern "C" void memory::paging::allocation::free_page(void* vaddress) {

    uint index = (uint64)vaddress / 4096;
    if (page_bitmap_map[index] == false) return;
    page_bitmap_map.set(index, false);
    free_memory_size += 4096;
    used_memory_size -= 4096;
}

extern "C" void memory::paging::allocation::free_pages(void* vaddress, uint64 page_count) {
 
    for (uint64 t = 0; t < page_count; t++)
        memory::paging::allocation::free_page((void*)((address)vaddress + (t * 4096)));
}

extern "C" void memory::paging::allocation::lock_page(void* vaddress) {
    uint index = (uint64)vaddress / 4096;
    if (page_bitmap_map[index] == true) return;
    page_bitmap_map.set(index, true);
    free_memory_size -= 4096;
    used_memory_size += 4096;
} 

extern "C" void memory::paging::allocation::lock_pages(void* vaddress, uint64 page_count) {

    for (uint t = 0; t < page_count; t++)
        memory::paging::allocation::lock_page((void*)((uint64)vaddress + (t * 4096)));
}

memory::paging::page_map_indexer::page_map_indexer(uint64 virtual_address) {

    virtual_address >>= 12;
    p_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pt_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pd_i = virtual_address & 0x1ff;
    virtual_address >>= 9;
    pdp_i = virtual_address & 0x1ff;
}

void memory::paging::map_memory(void *virtual_memory, void *physical_memory, bool quickwrite)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_memory);
    page_directory_entry pde;

    pde = pml_4->entries[indexer.pdp_i];
    page_table* pdp;
    if (!pde.get_flag(pt_flag::present)) {
    
        pdp = (page_table*)allocation::request_page();
        memory::operations::memset(pdp, 0, 0x1000);
        pde.set_address((uint64)pdp >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pml_4->entries[indexer.pdp_i] = pde;
        memory::paging::map_memory((void *)pdp, (void *)pdp, false);
    }
    else
        pdp = (page_table*)((uint64)pde.get_address() << 12);
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;
    if (!pde.get_flag(pt_flag::present)) {

        pd = (page_table*)allocation::request_page();
        memory::operations::memset(pd, 0, 0x1000);
        pde.set_address((uint64)pd >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pdp->entries[indexer.pd_i] = pde;
        memory::paging::map_memory((void *)pd, (void *)pd, false);
    }
    else
        pd = (page_table*)((uint64)pde.get_address() << 12);

    pde = pd->entries[indexer.pt_i];
    page_table* pt;
    if (!pde.get_flag(pt_flag::present)) {

        pt = (page_table*)allocation::request_page();
        memory::operations::memset(pt, 0, 0x1000);
        pde.set_address((uint64)pt >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pd->entries[indexer.pt_i] = pde;
        memory::paging::map_memory((void *)pt, (void *)pt, false);
    }
    else
        pt = (page_table*)((uint64)pde.get_address() << 12);


    pde = pt->entries[indexer.p_i];
    pde.set_address((uint64)physical_memory >> 12);
    pde.set_flag(pt_flag::present, true);
    pde.set_flag(pt_flag::read_write, true);
    pde.set_pat(quickwrite);
    pt->entries[indexer.p_i] = pde;
}

void* memory::paging::get_physical_address(void* virtual_address)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_address);
    page_directory_entry pde;

    uint16 address_offset = (uint16)(uint64)virtual_address & 0xFFF;
    virtual_address = (void *)~(uint64)virtual_address;

    pde = pml_4->entries[indexer.pdp_i];
    page_table* pdp;
    if (!pde.get_flag(pt_flag::present)) {
    
        pdp = (page_table*)allocation::request_page();
        memory::operations::memset(pdp, 0, 0x1000);
        pde.set_address((uint64)pdp >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pml_4->entries[indexer.pdp_i] = pde;
        memory::paging::map_memory((void *)pdp, (void *)pdp, false);
    }
    else
        pdp = (page_table*)((uint64)pde.get_address() << 12);
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;
    if (!pde.get_flag(pt_flag::present)) {

        pd = (page_table*)allocation::request_page();
        memory::operations::memset(pd, 0, 0x1000);
        pde.set_address((uint64)pd >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pdp->entries[indexer.pd_i] = pde;
        memory::paging::map_memory((void *)pd, (void *)pd, false);
    }
    else
        pd = (page_table*)((uint64)pde.get_address() << 12);

    pde = pd->entries[indexer.pt_i];
    page_table* pt;
    if (!pde.get_flag(pt_flag::present)) {

        pt = (page_table*)allocation::request_page();
        memory::operations::memset(pt, 0, 0x1000);
        pde.set_address((uint64)pt >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pd->entries[indexer.pt_i] = pde;
        memory::paging::map_memory((void *)pt, (void *)pt, false);
    }
    else
        pt = (page_table*)((uint64)pde.get_address() << 12);


    pde = pt->entries[indexer.p_i];

    if (!pde.get_flag(present))
        return nullptr;

    return (void *)(pde.get_address() | address_offset);
}

extern "C" void memory::paging::donate_to_userspace(void* virtual_address)
{
    page_map_indexer indexer = page_map_indexer((uint64)virtual_address);
    page_directory_entry pde;
    pde = pml_4->entries[indexer.pdp_i];
    pml_4->entries[indexer.pdp_i].set_flag(pt_flag::user_super, 1);
    page_table* pdp = (page_table*)((uint64)pde.get_address() << 12);
    pdp->entries[indexer.pd_i].set_flag(pt_flag::user_super, 1);
    pde = pdp->entries[indexer.pd_i];
    page_table* pd = (page_table*)((uint64)pde.get_address() << 12);
    pd->entries[indexer.pt_i].set_flag(pt_flag::user_super, 1);
    pde = pd->entries[indexer.pt_i];
    page_table* pt = (page_table*)((uint64)pde.get_address() << 12);
    pt->entries[indexer.p_i].set_flag(pt_flag::user_super, 1);
}

void memory::paging::page_directory_entry::set_flag(pt_flag flag, bool enabled) {

    uint64 bit_selector = (uint64)1 << flag;
    value &= ~bit_selector;
    if (enabled)
        value |= bit_selector;
}

bool memory::paging::page_directory_entry::get_flag(pt_flag flag) {

    uint64 bit_selector = (uint64)1 << flag;
    return (value & bit_selector) > 0 ? true : false;
}

uint64 memory::paging::page_directory_entry::get_address() {

    return (value & 0x000ffffffffff000) >> 12;
}

void memory::paging::page_directory_entry::set_address(address paddress) {

    paddress &= 0x000000ffffffffff;
    value &= 0xfff0000000000fff;
    value |= (paddress << 12);
}

void memory::paging::page_directory_entry::set_pat(uint08 index) {

    index &= 0b111;
    set_flag(pt_flag::write_through, index & 1);
    index >>= 1;
    set_flag(pt_flag::cache_disabled, index & 1);
    index >>= 1;
    set_flag(pt_flag::larger_pages, index & 1);
}

//////////////////////////////////////////
// memops
//////////////////////////////////////////

void *memory::operations::memcpy(void *__restrict dst, const void *__restrict src, uint64 count)
{
	asm volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dst) :: "memory");
	return dst;
}

void* memory::operations::memset(void* bufptr, byte value, uint64 size) {
	byte* buf = (byte *) bufptr;
	for (uint64 i = 0; i < size; i++)
		buf[i] = value;
	return bufptr;
}