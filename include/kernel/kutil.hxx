#pragma once
#include <stdint.h>
#include <stddef.h>

namespace util
{
	char* 						itoa(long int val, int base);
	static inline uint8_t 		get_bit(uint64_t source, uint8_t bit)
	{
		return (source >> bit) & 1;
	}
	size_t strlen(const char* str);
	bool strcomp(const char* lhs, const char *rhs);
	char * strcpy(char * __restrict dst, const char * __restrict src);
	char * strcat(char *dest, const char *src);
	int isdigit(int c);
}