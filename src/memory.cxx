//
// Created by rizet on 1/26/21.
//

#include "kernel/memory.hxx"
#include "kernel/boot.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include <cstdint>

//////////////////////////////////////////
// variables
//////////////////////////////////////////

size_t memory::memory_size;
memory::allocation::conventional_memory_chunk user_heap;
memory::allocation::conventional_memory_chunk kernel_heap;
void* kernel_heap_allocator_ptr;
void* user_heap_allocator_ptr;
memory::allocation::memory_info* meminfo;

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


void memory::allocation::map_memory(boot::memory_map_descriptor* memory_map, uint64_t map_size, uint64_t desc_size)
{
	memory::allocation::conventional_memory_chunk conventional_chunks[map_size / desc_size];
	uint8_t c_c_index = 0;

	uint8_t *startOfMemoryMap = (uint8_t *)memory_map;
	uint8_t *endOfMemoryMap = startOfMemoryMap + map_size;

	uint8_t *offset = startOfMemoryMap;

	uint64_t pages = 0;

	uint32_t counter = 0;

	for (unsigned int i = 0; i < (map_size / desc_size - 1); i++)
	{
		conventional_chunks[i].pages = 0;
		conventional_chunks[i].start = 0;
	}

	io::serial::serial_msg("\n\nMEMORY MAP:\n-=-=-=-=-=-\n");

	while (offset < endOfMemoryMap)
	{
		boot::memory_map_descriptor *desc = (boot::memory_map_descriptor *)offset;

		if   	(desc->type == 0x07
			  || desc->type == 0x01 || desc->type == 0x02
			  || desc->type == 0x04 || desc->type == 0x05)
					pages += desc->count;

		if (desc->type == 0x07 && desc->physical_start != 0x100000)
		{
			memory::allocation::conventional_memory_chunk cmchunk;
			cmchunk.start = (void *)desc->physical_start;
			cmchunk.pages = desc->count;
			conventional_chunks[c_c_index] = cmchunk;
			c_c_index++;
		}

		io::serial::serial_msg(util::itoa(desc->physical_start, 16));
		io::serial::serial_msg(": ");
		io::serial::serial_msg(memory_types[desc->type]);
		io::serial::serial_msg(", ");
		io::serial::serial_msg(util::itoa(desc->count, 10));
		io::serial::serial_msg(" pages");
		io::serial::serial_msg("\n");

		offset += desc_size;

		counter++;
	}

	for (memory::allocation::conventional_memory_chunk i : conventional_chunks)
	{
		if (i.start == nullptr)
			break;

		io::serial::serial_msg("chunk at ");
		io::serial::serial_msg(util::itoa((uint64_t)i.start, 16));
		io::serial::serial_msg(": ");
		io::serial::serial_msg(util::itoa(i.pages * 0x1000 / 1024, 10));
		io::serial::serial_msg(" kilobytes\n");
	}

	memory::allocation::conventional_memory_chunk* largest_memory_chunk;

	largest_memory_chunk->pages = 0;
	largest_memory_chunk->start = 0;

	for (int i = 0; conventional_chunks[i].start != nullptr; i++)
	{
		if (conventional_chunks[i].pages > largest_memory_chunk->pages)
			largest_memory_chunk = &conventional_chunks[i];
	}

	meminfo->user_heap = largest_memory_chunk->start;
	meminfo->user_heap_size = largest_memory_chunk->pages * 0x1000;

	largest_memory_chunk->pages = 0;

	for (int i = 0; conventional_chunks[i].start != nullptr; i++)
	{
		if (conventional_chunks[i].pages > largest_memory_chunk->pages)
			largest_memory_chunk = &conventional_chunks[i];
	}

	meminfo->kernel_heap = largest_memory_chunk->start;
	meminfo->kernel_heap_size = largest_memory_chunk->pages * 0x1000;

	meminfo->memory_size = pages * 0x1000;

	io::serial::serial_msg("Kernel heap: ");
	io::serial::serial_msg(util::itoa((uint64_t)meminfo->kernel_heap, 16));
	io::serial::serial_msg(", ");

	io::serial::serial_msg(util::itoa((int64_t)meminfo->kernel_heap_size / 1024, 10));
	io::serial::serial_msg(" kilobytes.\nUser heap: ");
	io::serial::serial_msg(util::itoa((uint64_t)meminfo->user_heap, 16));
	io::serial::serial_msg(", ");
	io::serial::serial_msg(util::itoa((int64_t)meminfo->user_heap_size / 1024, 10));
	io::serial::serial_msg(" kilobytes.\n");
}

void memory::allocation::start_malloc()
{
	memory_size = meminfo->memory_size;

	kernel_heap.start = meminfo->kernel_heap;
	kernel_heap.pages = meminfo->kernel_heap_size / 0x1000;
	user_heap.start = meminfo->user_heap;
	user_heap.pages = meminfo->user_heap_size / 0x1000;

	user_heap_allocator_ptr = user_heap.start;
	kernel_heap_allocator_ptr = kernel_heap.start;

	io::serial::serial_msg("User heap allocator pointer is at ");
	io::serial::serial_msg(util::itoa((uint64_t)meminfo->user_heap, 16));
	io::serial::serial_msg("\nKernel heap allocator pointer is at ");
	io::serial::serial_msg(util::itoa((uint64_t)meminfo->kernel_heap, 16));
	io::serial::serial_msg("\n");

	allocator_on = true;	
}

void* memory::allocation::malloc(size_t bytes)
{
	if (!allocator_on)
		return nullptr;

	io::serial::serial_msg("allocating ");
	io::serial::serial_msg(util::itoa(bytes, 10));
	io::serial::serial_msg(" byte(s) on user heap, at");	

	void* return_address = user_heap_allocator_ptr;

	io::serial::serial_msg(util::itoa((uint64_t)return_address, 16));
	io::serial::serial_msg("\n");

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

	io::serial::serial_msg("allocating ");
	io::serial::serial_msg(util::itoa(bytes, 10));
	io::serial::serial_msg(" byte(s) on kernel heap, at ");

	void* return_address = kernel_heap_allocator_ptr;

	io::serial::serial_msg(util::itoa((uint64_t)return_address, 16));
	io::serial::serial_msg("\n");

	kernel_heap_allocator_ptr = static_cast<char *>(kernel_heap_allocator_ptr) + bytes;	

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

volatile uint64_t pageDirectory[1024] __attribute__ ((aligned (4096)));
volatile uint64_t firstPageTable[1024] __attribute__ ((aligned (4096)));

extern "C" void setup_paging_s(void*);

void memory::paging::setup_paging(void *pageDirectory)
{ setup_paging_s(pageDirectory); }

void memory::paging::begin_paging() {
	for (auto i = 0; i < 1024; i++) {
		firstPageTable[i] = (i * 0x1000) | 3;
	}
	pageDirectory[0] = ((uint64_t)firstPageTable) | 3;
	memory::paging::setup_paging((void*)&pageDirectory);
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