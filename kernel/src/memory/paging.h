#pragma once
#include "types.h"
#include <stddef.h>
#include "boot/boot.h"
#include "util/kutil.h"
#include "drivers/io.h"

namespace memory {
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

		void map_memory(void* virtual_memory, void* physical_memory, bool quickwrite);
		extern "C" void donate_to_userspace(void* virtual_address);
		void* get_physical_address(void* virtual_address);
	}
}