#pragma once
#include "stdint.h"
#include "stddef.h"

extern "C" void beginPaging();
extern "C" void memset(void* destination, uint32_t value, size_t length);
extern "C" void setupPaging(void* pageDirectory);
extern "C" void* malloc(size_t bytes);
extern "C" void free(void* data);