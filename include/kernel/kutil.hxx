#pragma once
#include <stdint.h>

char getChar(uint8_t keycode);
void writeHex(uint32_t num);
extern "C" void int2serial_hex(uint64_t num);
extern "C" void int2serial_any(int64_t value, int base);
char* itoa(int val, int base);

typedef union uint64_bytearr
{
	unsigned char b[8];
	uint64_t i;
} u64_byte_arr;

static inline uint8_t get_bit(uint64_t source, uint8_t bit)
{
	return (source >> bit) & 1;
}