#pragma once

#include "stddef.h"

extern "C" {

void loadGDT();
void loadIDT();
void initIDT();
void memset(void* destination, int value, size_t length);
void setupPaging(void* pageDirectory);

}
