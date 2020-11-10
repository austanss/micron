#pragma once
#include <stdint.h>

char getChar(uint8_t keycode);
void writeHex(uint32_t num);
void hex_str_serial(uint32_t num);

typedef union uint64_bytearr
{
	unsigned char b[sizeof(uint64_t)];
	uint64_t i;
} u64_byte_arr;