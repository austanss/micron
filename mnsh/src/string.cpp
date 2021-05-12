#include "string.h"

const char itoa_characters[] = "0123456789abcdef";

char itoa_buf[64] = { 0 };

char* itoa(long int val, int base)
{
	int i = 60;

	for(; val && i ; --i, val /= base)

		itoa_buf[i] = itoa_characters[val % base];

	return &itoa_buf[i+1];
}

bool strcmp(const char* lhs, const char *rhs)
{
    const char* slhs = lhs;

	for (; *lhs; lhs++)
	{
		if (*lhs != *rhs)
			return false;

		rhs++;
	}

    if (!*slhs) return false;

	return true;
}

void* memset(void* buffer, unsigned char value, unsigned long count)
{
	unsigned char* cbuffer = (unsigned char *)buffer;
	for (unsigned long i = 0; i < count; i++)
		cbuffer[i] = value;
	return buffer;
}