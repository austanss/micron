#include "KernelUtil.h"
#include "Terminal.h"
#include <stdint.h>

char getChar(uint8_t keycode)
{
        switch (keycode)
        {
                case 0x49101:
                        return 'a';
                case 159:
                        return 's';
                case 160:
                        return 'd';
                case 161:
                        return 'f';
		default:
			return ' ';
        }
}

void writeHex(int n)
{
  	int tmp;

  	Terminal::instance().write("0x");

  	bool noZeroes = true;

  	int i;
  	for (i = 28; i > 0; i -= 4)
  	{
    		tmp = (n >> i) & 0xF;
    		if (tmp == 0 && noZeroes)
    		{
      			continue;
    		}

    		if (tmp >= 0xA)
    		{
      			noZeroes = false;
      			Terminal::instance().write(tmp-0xA+'a');
		}
    		else
    		{
      			noZeroes = false;
      			Terminal::instance().write(tmp+'0');
    		}
  	}

  	tmp = n & 0xF;
  	if (tmp >= 0xA)
  	{
  	  	Terminal::instance().write(tmp-0xA+'a');
  	}
  	else
  	{
    		Terminal::instance().write(tmp+'0');
	}

}
