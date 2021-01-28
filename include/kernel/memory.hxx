#pragma once
#include "stdint.h"
#include "stddef.h"
#include "kernel/bootinfo.hxx"
#include "kernel/kutil.hxx"
#include "kernel/io.hxx"

extern size_t memory_size;

typedef struct s_memory_info {
	size_t 				memory_size; // bytes
	void* 				kernel_heap;
	size_t 		   kernel_heap_size; // bytes
	void*				  user_heap;
	size_t 			 user_heap_size; // bytes
} memory_info;

typedef struct s_conv_mem_chunk {
	void* start;
	size_t pages;
} conventional_memory_chunk;

extern "C" void 	begin_paging();
extern "C" void* 	memset(void* bufptr, int value, size_t size);
extern "C" void 	setup_paging(void* pageDirectory);
extern "C" void* 	malloc(size_t bytes);
extern "C" void 	free(void* data);
extern "C" void* 	kmalloc(size_t bytes);
extern "C" void 	kfree(void* data);
extern "C" void 	*memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size);
void		 		map_memory(Memory_Map_Descriptor* memmap, uint64_t map_size, uint64_t desc_size);
void 				start_memory_manager();
