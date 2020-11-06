#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include <stdint.h>

char* hex_str(uint32_t num)
{
	char buffer[11];
	buffer[0] = '0';
	buffer[1] = 'x';
	for (int i = 7; i >= 0; i--)
	{
		buffer[i+2] = static_cast<char>(num % 16 < 10 ? '0' + num % 16 : 'A' - 10 + num % 16);
		num /= 16;
	}
	buffer[10]= '\0';         /* terminate string */

	char* final = (char *)0xDEADCAFE;

	for (int i = 0; i < 11; i++)
	{
		final[i] = buffer[i];
	}

	return final;
}

void writeHex(uint32_t num)
{
    Terminal::instance().write(hex_str(num));     /* print it */
}
