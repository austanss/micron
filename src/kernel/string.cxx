//
// Created by rizet on 10/27/20.
//
#include "Strings.hxx"

bool strcomp(const char* lhs, const char *rhs)
{
	for (; *lhs; lhs++)
	{
		if (*lhs != *rhs)
			return false;

		rhs++;
	}
	return true;
}

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len] && str[len] != '\0')
		len++;
	return len;
}