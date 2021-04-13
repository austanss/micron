#include "operations.h"

void *memory::operations::memcpy(void *__restrict dst, const void *__restrict src, uint64 count)
{
	asm volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dst) :: "memory");
	return dst;
}

void* memory::operations::memset(void* bufptr, byte value, uint64 size) {
	byte* buf = (byte *) bufptr;
	for (uint64 i = 0; i < size; i++)
		buf[i] = value;
	return bufptr;
}