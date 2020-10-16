#include <stdint.h>
#include "AssemblyFunctions.h"
#include "Memory.h"

using PageDirectoryEntry = uint32_t;
using PageTableEntry = uint32_t;

volatile PageDirectoryEntry pageDirectory[1024] __attribute__ ((aligned (4096)));
volatile PageTableEntry firstPageTable[1024] __attribute__ ((aligned (4096)));

void MemoryManagement::beginPaging() {
	for (auto i = 0; i < 1024; i++) {
		firstPageTable[i] = (i * 0x1000) | 3;
	}
	pageDirectory[0] = ((unsigned int) firstPageTable) | 3;
	setupPaging((void*)&pageDirectory);
}
