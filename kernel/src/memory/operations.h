#include "types.h"

namespace memory {
	namespace operations {	
		void*	memcpy(void* __restrict dst, const void* __restrict src, uint64 size);
		void* 	memset(void* dst, byte value, uint64 size);
	}
}

using namespace memory::operations;