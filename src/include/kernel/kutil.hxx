#pragma once
#include <stdint.h>

char getChar(uint8_t keycode);
void writeHex(uint32_t num);
void hex_str_serial(uint64_t num);

typedef union uint64_bytearr
{
	unsigned char b[8];
	uint64_t i;
} u64_byte_arr;