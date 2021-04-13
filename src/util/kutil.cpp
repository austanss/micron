#include "util/kutil.h"
#include "userio/tty.h"
#include "devio/io.h"

char* util::itoa(long int val, int base)
{
	static char buf[64] = {0};

	sint i = 60;

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

uint util::strlen(const char* str)
{
	uint len = 0;
	while (str[len] && str[len] != '\0')
		len++;
	return len;
}

char* util::strcat(char *dest, const char *src) {

	char * end = dest;
	while (*end != '\0') {
		++end;
	}
	while (*src) {
		*end = *src;
		end++;
		src++;
	}
	*end = '\0';
	return dest;
}

char* util::strcpy(char * __restrict dst, const char * __restrict src) {
	
	char * out = dst;
	for (; (*dst=*src); src++, dst++);
	return out;
}

int util::isdigit(char c) {
	
	return (c >= '0' && c <= '9');
}