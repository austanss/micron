#pragma once
#include "types.h"
#include <stddef.h>
#include "boot/boot.h"
#include "util/kutil.h"
#include "devio/io.h"

namespace memory {

	extern uint64 free_memory_size;
	extern uint64 total_memory_size;
	extern uint64 used_memory_size;
	extern uint64 reserved_memory_size;

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
			uint64 value;
			void set_flag(pt_flag flag, bool enabled);
			bool get_flag(pt_flag flag);
			void set_pat(uint08 index);
			void set_address(uint64 address);
			uint64 get_address();
		};

		struct page_table { 
			page_directory_entry entries [512];
		}__attribute__((aligned(0x1000)));

		class page_map_indexer {
		public:
			page_map_indexer(uint64 virtual_address);
			uint64 pdp_i;
			uint64 pd_i;
			uint64 pt_i;
			uint64 p_i;
		};

		extern page_table* pml_4;
		extern page_table* pml_4_id_full;

		void map_memory(void* virtual_memory, void* physical_memory, bool quickwrite);
		extern "C" void donate_to_userspace(void* virtual_address);
		void* get_physical_address(void* virtual_address);

		namespace allocation {
			extern "C" void 	free_page(void* address);
    		extern "C" void 	free_pages(void* address, uint64 page_count);
    		extern "C" void 	lock_page(void* address);
    		extern "C" void		lock_pages(void* address, uint64 page_count);
   			extern "C" void* 	request_page();
			extern "C" void*	request_pages(uint64 page_count);   
		}
	}

	namespace allocation {
		struct memory_info {
			uint64 				memory_size; // bytes
			void* 				kernel_heap;
			uint64 		   kernel_heap_size; // bytes
			void*				  user_heap;
			uint64 			 user_heap_size; // bytes
		};

		struct heap_segment_header {
    		uint64 length;
    		heap_segment_header* next;
    		heap_segment_header* last;
    		bool free;
    		void combine_forward();
    		void combine_backward();
    		heap_segment_header* split(uint64 split_length);
		};

		void* 		malloc(uint64 bytes);
		void 		free(void* data);
		void 		initialize_heap(void* heap_address, uint64 heap_length);
		void		map_memory(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size);
		uint64		get_total_memory_size(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size);
	}

	namespace operations {	
		void*	memcpy(void* __restrict dst, const void* __restrict src, uint64 size);
		void* 	memset(void* buf, byte value, uint64 size);
	}
}