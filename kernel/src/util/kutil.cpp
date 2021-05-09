#include "util/kutil.h"
#include "drivers/tty/tty.h"
#include "drivers/io.h"
#include "memory/heap.h"

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

	char* end = dest;

	while (*end != '\0')
		++end;

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

char* util::strdup(char* source)
{
	char* dst = (char *)memory::heap::malloc(strlen(source) + 1);
	strcpy(dst, source);
	return dst;
}

char** util::strsplit(char* src, char delimiter)
{
	char* source = strdup(src);

	int delimiter_occurences = 0;

	for (char* source_iterator = source; *source_iterator; source_iterator++)
		if (*source_iterator == delimiter)
			delimiter_occurences++;

	char** split_strings = (char **)memory::heap::malloc(sizeof(char **) * delimiter_occurences);

	split_strings[0] = source;

	int split_strings_index = 1;

	for (uint i = 0; i < strlen(source); i++)
	{
		if (source[i] == delimiter && split_strings_index <= delimiter_occurences)
		{
			source[i] = 0;
			split_strings[split_strings_index] = source + i + 1;
			split_strings_index++;
		}	
	}

	return split_strings;
}