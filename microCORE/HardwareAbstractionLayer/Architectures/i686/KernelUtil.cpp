#include "KernelUtil.h"
#include "Terminal.h"
#include <stdint.h>

void writeHex(uint32_t num)
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

    Terminal::instance().write(buffer);     /* print it */
}
