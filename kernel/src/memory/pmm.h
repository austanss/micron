#include "types.h"
#include "boot/boot.h"

namespace memory {
	namespace pmm {
		extern uint free_memory_size;
		extern uint used_memory_size;
		extern uint total_memory_size;
		extern uint reserved_memory_size;

		void 				initialize(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size);
		uint 				get_total_memory_size(stivale_memory_map* memory_map, uint64 map_size, uint64 desc_size);

		extern "C" void*	reallocate_pool(void* address, uint64 new_page_count);
		extern "C" void 	free_page(void* address);
   		extern "C" void 	free_pool(void* address);
   		extern "C" void 	lock_page(void* address);
   		extern "C" void		lock_pages(void* address, uint64 page_count);
		extern "C" void* 	request_page();
		extern "C" void*	request_pool(uint64 page_count);   
	}
}