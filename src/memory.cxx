//
// Created by rizet on 1/26/21.
//

#include "kernel/memory.hxx"

// malloc


// paging

volatile uint64_t pageDirectory[1024] __attribute__ ((aligned (4096)));
volatile uint64_t firstPageTable[1024] __attribute__ ((aligned (4096)));

void beginPaging() {
	for (auto i = 0; i < 1024; i++) {
		firstPageTable[i] = (i * 0x1000) | 3;
	}
	pageDirectory[0] = ((uint64_t)firstPageTable) | 3;
	setupPaging((void*)&pageDirectory);
}