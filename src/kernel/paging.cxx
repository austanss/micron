#include <stdint.h>
#include "kernel/asmfuncs.hxx"
#include "kernel/mem.hxx"

using PageDirectoryEntry = uint64_t;
using PageTableEntry = uint64_t;

volatile PageDirectoryEntry pageDirectory[1024] __attribute__ ((aligned (4096)));
volatile PageTableEntry firstPageTable[1024] __attribute__ ((aligned (4096)));

void beginPaging() {
	for (auto i = 0; i < 1024; i++) {
		firstPageTable[i] = (i * 0x1000) | 3;
	}
	pageDirectory[0] = ((uint64_t)firstPageTable) | 3;
	setupPaging((void*)&pageDirectory);
}
