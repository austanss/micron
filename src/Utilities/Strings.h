#pragma once

#include "stddef.h"

size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len] && str[len] != '\0')
		len++;
	return len;
}

bool strcomp(const char* lhs, const char *rhs) {
	for (; *lhs; lhs++) {
		if (*lhs != *rhs) {
			return false;
		}
		rhs++;
	}
	return true;
}
