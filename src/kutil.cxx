#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/io.hxx"
#include <stdint.h>

char* util::itoa(long int val, int base)
{
	static char buf[64] = {0};

	int i = 60;

	for(; val && i ; --i, val /= base)

		buf[i] = "0123456789abcdef"[val % base];

	return &buf[i+1];
}


bool util::strcomp(const char* lhs, const char *rhs)
{
	for (; *lhs; lhs++)
	{
		if (*lhs != *rhs)
			return false;

		rhs++;
	}
	return true;
}

size_t util::strlen(const char* str)
{
	size_t len = 0;
	while (str[len] && str[len] != '\0')
		len++;
	return len;
}