#pragma once
#include "stdint.h"
#include "stddef.h"
#include "kernel/boot.hxx"
#include "kernel/kutil.hxx"
#include "kernel/io.hxx"

namespace memory {

	extern size_t memory_size;

	namespace paging {	
	 	void 	begin_paging();
		void 	setup_paging(void* pageDirectory);
	};

	namespace allocation {
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

		void* 	malloc(size_t bytes);
		void 	free(void* data);
		void* 	kmalloc(size_t bytes);
		void 	kfree(void* data);
		void 	start_malloc();
		void	map_memory(boot::memory_map_descriptor* memory_map, uint64_t map_size, uint64_t desc_size);
	};

	namespace operations {	
		void*	memcpy(void* __restrict dst, const void* __restrict src, size_t size);
		void* 	memset(void* buf, int value, size_t size);
	};
};