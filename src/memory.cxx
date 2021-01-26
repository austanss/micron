//
// Created by rizet on 1/26/21.
//

#include "kernel/memory.hxx"

// variables
size_t memory_size;

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

// malloc
memory_info* map_memory(Memory_Map_Descriptor* memmap, uint64_t map_size, uint64_t desc_size)
{
	memory_info* meminfo;

	conventional_memory_chunk conventional_chunks[map_size / desc_size];
	uint8_t c_c_index = 0;

	uint8_t *startOfMemoryMap = (uint8_t *)memmap;
	uint8_t *endOfMemoryMap = startOfMemoryMap + map_size;

	uint8_t *offset = startOfMemoryMap;

	uint64_t pages = 0;

	uint32_t counter = 0;
	serial_msg("\n\nMEMORY MAP:\n-=-=-=-=-=-\n");

	while (offset < endOfMemoryMap)
	{
		Memory_Map_Descriptor *desc = (Memory_Map_Descriptor *)offset;

		if   	(desc->type == 0x07
			  || desc->type == 0x01 || desc->type == 0x02
			  || desc->type == 0x04 || desc->type == 0x05)
					pages += desc->count;

		if (desc->type == 0x07)
		{
			conventional_memory_chunk cmchunk;
			cmchunk.start = (void *)desc->physical_start;
			cmchunk.pages = desc->count;
			conventional_chunks[c_c_index] = cmchunk;
			c_c_index++;
		}

		int2serial_hex(desc->physical_start);
		serial_msg(": ");
		serial_msg(memory_types[desc->type]);
		serial_msg(", ");
		serial_msg(itoa(desc->count, 10));
		serial_msg(" pages");
		serial_msg("\n");

		offset += desc_size;

		counter++;
	}

	meminfo->memory_size = pages * 0x1000;

	return meminfo;
}

void start_memory_manager(memory_info* mem_info)
{
	memory_size = mem_info->memory_size;
}

// paging
volatile uint64_t pageDirectory[1024] __attribute__ ((aligned (4096)));
volatile uint64_t firstPageTable[1024] __attribute__ ((aligned (4096)));

void begin_paging() {
	for (auto i = 0; i < 1024; i++) {
		firstPageTable[i] = (i * 0x1000) | 3;
	}
	pageDirectory[0] = ((uint64_t)firstPageTable) | 3;
	setup_paging((void*)&pageDirectory);
}
