#pragma once

#include "stdint.h"

extern "C" {

struct __attribute__((packed)) IDTEntry
{
	volatile uint16_t base_lower;
	volatile uint16_t sel;
	volatile uint8_t  always0;
	volatile uint8_t  flags;
	volatile uint16_t base_upper;
};

struct __attribute__((packed)) IDTPointer
{
	volatile uint16_t limit;
	volatile uint32_t base;
};

struct __attribute__((packed)) GDTEntry
{
	volatile uint16_t limit_low;
	volatile uint16_t base_low;
	volatile uint8_t  base_middle;
	volatile uint8_t  access;
	volatile uint8_t  granularity;
	volatile uint8_t  base_high;
};

struct __attribute__((packed)) GDTPointer
{
	volatile uint16_t limit;
	volatile uint32_t base;
};

}
