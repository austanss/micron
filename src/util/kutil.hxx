#pragma once
#include "types.hxx"

namespace util
{
	char* 						itoa(long int val, int base);
	static inline uint08 		get_bit(uint64 source, uint08 bit)
	{
		return (source >> bit) & 1;
	}
	uint strlen(const char* str);
	bool strcomp(const char* lhs, const char *rhs);
	char * strcpy(char * __restrict dst, const char * __restrict src);
	char * strcat(char *dest, const char *src);
	int isdigit(char c);
}