#pragma once
#include "stdint.h"
#include "stddef.h"
#include "kernel/boot.hxx"
#include "kernel/kutil.hxx"
#include "kernel/io.hxx"

namespace memory {

	extern size_t free_memory_size;
	extern size_t total_memory_size;
	extern size_t used_memory_size;
	extern size_t reserved_memory_size;

	namespace paging {	
		enum pt_flag {
			present = 0,
			read_write = 1,
			user_super = 2,
			write_through = 3,
			cache_disabled = 4,
			accessed = 5,
			larger_pages = 7,
			custom_0 = 9,
			custom_1 = 10,
			custom_2 = 11,
			nx = 63 // only if supported
		};	
		struct page_directory_entry {
			uint64_t value;
			void set_flag(pt_flag flag, bool enabled);
			bool get_flag(pt_flag flag);
			void set_address(uint64_t address);
			uint64_t get_address();
		};

		struct page_table { 
			page_directory_entry entries [512];
		}__attribute__((aligned(0x1000)));

		class page_map_indexer {
		public:
			page_map_indexer(uint64_t virtual_address);
			uint64_t pdp_i;
			uint64_t pd_i;
			uint64_t pt_i;
			uint64_t p_i;
		};

		extern page_table* pml_4;
		extern page_table* pml_4_id_full;

		void map_memory(void* virtual_memory, void* physical_memory);

		namespace allocation {
			void 	free_page(void* address);
    		void 	free_pages(void* address, uint64_t page_count);
    		void 	lock_page(void* address);
    		void	lock_pages(void* address, uint64_t page_count);
   			void* 	request_page();
			void*	request_pages(uint64_t page_count);   
		}
	};

	namespace allocation {
		struct memory_info {
			size_t 				memory_size; // bytes
			void* 				kernel_heap;
			size_t 		   kernel_heap_size; // bytes
			void*				  user_heap;
			size_t 			 user_heap_size; // bytes
		};

		struct heap_segment_header {
    		size_t length;
    		heap_segment_header* next;
    		heap_segment_header* last;
    		bool free;
    		void combine_forward();
    		void combine_backward();
    		heap_segment_header* split(size_t split_length);
		};

		void* 		malloc(size_t bytes);
		void 		free(void* data);
		void 		initialize_heap(void* heap_address, size_t heap_length);
		void		map_memory(stivale_memory_map* memory_map, uint64_t map_size, uint64_t desc_size);
		uint64_t	get_total_memory_size(stivale_memory_map* memory_map, uint64_t map_size, uint64_t desc_size);
	};

	namespace operations {	
		void*	memcpy(void* __restrict dst, const void* __restrict src, size_t size);
		void* 	memset(void* buf, int value, size_t size);
	};
};