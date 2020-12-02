#pragma once

#include "stddef.h"
#include "stdint.h"

extern "C" {

void loadGDT();
void loadIDT();
void initIDT();
void memset(void* destination, uint32_t value, size_t length);
void setupPaging(void* pageDirectory);

}
