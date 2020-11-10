#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/io.hxx"
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

	return buffer;
}

void writeHex(uint32_t num)
{
    Terminal::instance().write(hex_str(num));     /* print it */
}

void hex_str_serial(uint64_t num)
{
	char buffer[19];
	buffer[0] = '0';
	buffer[1] = 'x';
	for (int i = 15; i >= 0; i--)
	{
		buffer[i+2] = static_cast<char>(num % 16 < 10 ? '0' + num % 16 : 'A' - 10 + num % 16);
		num /= 16;
	}
	buffer[18]= '\0';         /* terminate string */

	serial_msg(buffer);
}
