#include "types.h"
#include "boot/boot.h"

namespace memory {
	namespace heap {
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
		void		expand_heap(uint64 size);
	}
}

using namespace memory::heap;