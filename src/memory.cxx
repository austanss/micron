//
// Created by rizet on 1/26/21.
//
#include <cstddef>
#include <stdint.h>
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
memory::allocation::conventional_memory_chunk user_heap;
memory::allocation::conventional_memory_chunk kernel_heap;
memory::allocation::conventional_memory_chunk page_bitmap;
util::bitmap page_bitmap_map;
char* kernel_heap_allocator_ptr;
char* user_heap_allocator_ptr;
bool initialized = false;
memory::allocation::memory_info* mem_info;
memory::paging::page_table* memory::paging::pml_4;

bool allocator_on = false;

const char *memory_types[] =
{
	"EfiReservedMemoryType",
	"EfiLoaderCode",
	"EfiLoaderData",
	"EfiBootServicesCode",
	"EfiBootServicesData",
	"EfiRuntimeServicesCode",
	"EfiRuntimeServicesData",
	"EfiConventionalMemory",
	"EfiUnusableMemory",
	"EfiACPIReclaimMemory",
	"EfiACPIMemoryNVS",
	"EfiMemoryMappedIO",
	"EfiMemoryMappedIOPortSpace",
	"EfiPalCode",
};

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

uint64_t memory::allocation::get_total_memory_size(boot::memory_map_descriptor* memory_map, uint64_t map_size, uint64_t desc_size)
{
    static uint64_t memory_size_bytes = 0;
    if (memory_size_bytes > 0) 
		return memory_size_bytes;

    for (uint64_t i = 0; i < map_size / desc_size; i++){
        boot::memory_map_descriptor* desc = (boot::memory_map_descriptor*)((uint64_t)memory_map + (i * desc_size));
        memory_size_bytes += desc->count * 4096;
    }

    return memory_size_bytes;

}

void reserve_pages(void* address, uint64_t page_count);

void memory::allocation::map_memory(boot::memory_map_descriptor* memory_map, const uint64_t map_size, const uint64_t desc_size)
{
    if (initialized) return;

    initialized = true;

    uint64_t map_entries = map_size / desc_size;

    void* largest_free_memory_segment = NULL;
    size_t largest_free_memory_segment_size = 0;

    for (uint64_t i = 0; i < map_entries; i++){
        boot::memory_map_descriptor* desc = (boot::memory_map_descriptor*)((uint64_t)memory_map + (i * desc_size));
        if (desc->type == 7){ // type = EfiConventionalMemory
            if (desc->count * 4096 > largest_free_memory_segment_size)
            {
                largest_free_memory_segment = (void *)desc->physical_start;
                largest_free_memory_segment_size = desc->count * 4096;
            }
        }
    }

    uint64_t memory_size = get_total_memory_size(memory_map, map_size, desc_size);
    free_memory_size = memory_size;
    uint64_t bitmap_size = memory_size / 4096 / 8 + 1;

    init_bitmap(bitmap_size, largest_free_memory_segment);

    memory::paging::allocation::lock_pages(&page_bitmap_map, page_bitmap_map.size / 4096 + 1);

    for (uint64_t i = 0; i < map_entries; i++){
        boot::memory_map_descriptor* desc = (boot::memory_map_descriptor*)((uint64_t)memory_map + (i * desc_size));
        if (desc->type != 7){ // not efiConventionalMemory
            reserve_pages((void *)desc->physical_start, desc->count);
        }
    }

    reserve_pages((void *)0x0, 0x10000 / 0x1000);
}

void memory::allocation::start_malloc()
{

	kernel_heap.start = mem_info->kernel_heap;
	kernel_heap.pages = mem_info->kernel_heap_size / 0x1000;
	user_heap.start = mem_info->user_heap;
	user_heap.pages = mem_info->user_heap_size / 0x1000;

	user_heap_allocator_ptr = (char *)user_heap.start;
	kernel_heap_allocator_ptr = (char *)kernel_heap.start;

	allocator_on = true;	
}

void* memory::allocation::malloc(size_t bytes)
{
	if (!allocator_on)
		return nullptr;


	if ((uint64_t)user_heap_allocator_ptr % 2 != 0)
		user_heap_allocator_ptr++;

	char* return_address = user_heap_allocator_ptr;

	user_heap_allocator_ptr = static_cast<char *>(user_heap_allocator_ptr) + bytes;	

	return return_address;
}

void memory::allocation::free(void* data)
{
	if (!allocator_on)
		return;

	return;	
}

void* memory::allocation::kmalloc(size_t bytes)
{
	if (!allocator_on)
		return nullptr;

	if ((uint64_t)kernel_heap_allocator_ptr % 2 != 0)
		kernel_heap_allocator_ptr++;

	char* return_address = kernel_heap_allocator_ptr;

	kernel_heap_allocator_ptr = static_cast<char *>(kernel_heap_allocator_ptr) + bytes;	

	if ((uint64_t)return_address % 2 != 0)
		return_address++;

	return return_address;
}

void memory::allocation::kfree(void* data)
{
	if (!allocator_on)
		return;
	
	return;	
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

void reserve_page(void* address){
    uint64_t index = (uint64_t)address / 4096;

    if (page_bitmap_map[index] == true) 
		return;

    page_bitmap_map.set(index, true);
    memory::free_memory_size -= 4096;
    memory::reserved_memory_size += 4096;
}

void reserve_pages(void* address, uint64_t pageCount) {

    for (size_t t = 0; t < pageCount; t++)
        reserve_page((void*)((uint64_t)address + (t * 4096)));
}

void* memory::paging::allocation::request_page() {

    for (uint64_t index = 0; index < page_bitmap_map.size * 8; index++) {

        if (page_bitmap_map[index] == true) 
			continue;

        memory::paging::allocation::lock_page((void*)(index * 4096));

        return (void*)(index * 4096);
    }

    io::serial::serial_msg("requesting page, no pages available, returning nullptr\n");

    return nullptr;
}

void memory::paging::allocation::free_page(void* address) {

    uint64_t index = (uint64_t)address / 4096;
    if (page_bitmap_map[index] == false) return;
    page_bitmap_map.set(index, false);
    free_memory_size += 4096;
    used_memory_size -= 4096;
}

void memory::paging::allocation::free_pages(void* address, uint64_t page_count) {
 
    for (size_t t = 0; t < page_count; t++)
        memory::paging::allocation::free_page((void*)((uint64_t)address + (t * 4096)));
}

void memory::paging::allocation::lock_page(void* address){
    uint64_t index = (uint64_t)address / 4096;
    if (page_bitmap_map[index] == true) return;
    page_bitmap_map.set(index, true);
    free_memory_size -= 4096;
    used_memory_size += 4096;
}

void memory::paging::allocation::lock_pages(void* address, uint64_t page_count) {

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
    if (!pde.get_flag(pt_flag::present)){
        pdp = (page_table*)allocation::request_page();
        memory::operations::memset(pdp, 0, 0x1000);
        pde.set_address((uint64_t)pdp >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pml_4->entries[indexer.pdp_i] = pde;
    }
    else
    {
        pdp = (page_table*)((uint64_t)pde.get_address() << 12);
    }
    
    
    pde = pdp->entries[indexer.pd_i];
    page_table* pd;
    if (!pde.get_flag(pt_flag::present)){
        pd = (page_table*)allocation::request_page();
        memory::operations::memset(pd, 0, 0x1000);
        pde.set_address((uint64_t)pd >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pdp->entries[indexer.pd_i] = pde;
    }
    else
    {
        pd = (page_table*)((uint64_t)pde.get_address() << 12);
    }

    pde = pd->entries[indexer.pt_i];
    page_table* pt;
    if (!pde.get_flag(pt_flag::present)){
        pt = (page_table*)allocation::request_page();
        memory::operations::memset(pt, 0, 0x1000);
        pde.set_address((uint64_t)pt >> 12);
        pde.set_flag(pt_flag::present, true);
        pde.set_flag(pt_flag::read_write, true);
        pd->entries[indexer.pt_i] = pde;
    }
    else
    {
        pt = (page_table*)((uint64_t)pde.get_address() << 12);
    }

    pde = pt->entries[indexer.p_i];
    pde.set_address((uint64_t)physical_memory >> 12);
    pde.set_flag(pt_flag::present, true);
    pde.set_flag(pt_flag::read_write, true);
    pt->entries[indexer.p_i] = pde;
}

void memory::paging::page_directory_entry::set_flag(pt_flag flag, bool enabled) {

    uint64_t bit_selector = (uint64_t)1 << flag;
    value &= ~bit_selector;
    if (enabled)
        value |= bit_selector;
}

bool memory::paging::page_directory_entry::get_flag(pt_flag flag) {

    uint64_t bit_selector = (uint64_t)1 << flag;
    return value & bit_selector > 0 ? true : false;
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