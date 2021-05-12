#include "tty.h"
#include "string.h"
#include "sysinfo.h"

unsigned short* buffer;
unsigned int    width;
unsigned int    height;
unsigned short  row; 
unsigned short  col;
unsigned int*   fastfbb;
unsigned short  fastfbw;
unsigned short  fastfbh;

extern const unsigned long kernel_font[];

unsigned int bpp4_to_bpp32(unsigned char bpp4_color)
{
	unsigned int color = 0;
	switch (bpp4_color) 
	{
		case black: color = 0x000000; break;
		case white: color = 0xFFFFFF; break;
		case red: color = 0xFF0000; break;
		case blue: color = 0x2222FF; break;
		case green: color = 0x22FF22; break;
		case cyan: color = 0x11FFFF; break;
		case magenta: color = 0xFF01AA; break;
		case brown: color = 0xFFEBCD; break;
		case light_gray: color = 0xDDDDDD; break;
		case dark_gray: color = 0x555555; break;
		case light_blue: color = 0x01AAFF; break;
		case light_green: color = 0x01FF01; break;
		case light_cyan: color = 0x01DDFF; break;
		case light_red: color = 0xFF2222; break;
		case light_magenta: color = 0xFF0077; break;
		case light_brown: color = 0x8B4513; break;
	}

	return color;
}

void tty_initialize()
{
    memset((void *)get_info()->display_info.fb_ad, 0x00, get_info()->display_info.fb_size);
    buffer = (unsigned short *)pmap(0xdb800000);

    width = (get_info()->display_info.fb_resx / 8);
    height = (get_info()->display_info.fb_resy / 9);

	for (unsigned long offset = 0x1000; offset < (width * height * 2 / 0x1000 + 1) * 0x1000; offset += 0x1000)
		pmap(0xdb800000 + offset);

    col = 0;
    row = 0;

    fastfbb = (unsigned int *)get_info()->display_info.fb_ad;
	fastfbw = get_info()->display_info.fb_resx;
	fastfbh = get_info()->display_info.fb_resy;

    memset(buffer, 0x00, width * height * 2);
}

void tty_clear()
{
    memset((void *)get_info()->display_info.fb_ad, 0x00, get_info()->display_info.fb_size);
    memset(buffer, 0x00, width * height * 2);

    col = 0;
    row = 0;
}

void tty_seek(unsigned short x, unsigned short y)  
{
    if (col >= width || row >= height)
        return;

    col = x;
    row = y;
}

inline void ptty_plot_pixel(unsigned short x, unsigned short y, unsigned int pixel)
{ 	fastfbb[y * fastfbw + x] = pixel;	}
 
inline void ptty_put_entry(char c, unsigned char color, unsigned short x, unsigned short y)
{
    buffer[y * width + x] = c | (color << 8);
}

void ptty_render_entry(unsigned short x, unsigned short y)
{
  	unsigned int fg_color;
  	unsigned int bg_color;

	unsigned short entry = buffer[(y * width + x)];

	unsigned char bpp4_color_fg = (entry & 0x0f00) >> 8;	
	unsigned char bpp4_color_bg = (entry >> 12);
	
	char c = entry ^ 0xFF00;

	fg_color = bpp4_to_bpp32(bpp4_color_fg);
	bg_color = bpp4_to_bpp32(bpp4_color_bg);

    unsigned long font_selector = kernel_font[c];

    unsigned char bits[64];

	for (unsigned char i = 0; i < 64; i++) {
		bits[i] = (font_selector >> i) & 1;
	}

	for (unsigned int iy = 0, iyy = (y * 9); iy < 8; iy++, iyy++) {
		for (unsigned int ix = 0, ixx = (x * 8); ix < 8; ix++, ixx++) {
			if (bits[(8 * iy) + ix]) {
				ptty_plot_pixel(ixx, iyy, fg_color);
			} else {
				ptty_plot_pixel(ixx, iyy, bg_color);
			}
		}
	}
} 

void ptty_rerender()
{
    for (unsigned int y = 0; y < height; y++)
        for (unsigned int x = 0; x < width; x++)
            ptty_render_entry(x, y);
}

void tty_putc(char c, unsigned char color)
{
    if (!c)
        return;

    if (c == '\n')
    {
        if (row + 1 >= height)
            tty_shift(1);
        else
            row++;

        col = 0;
        return;
    }

    if (c == '\t')
    {
        col = (col / 4 + 1) * 4;
        return;
    }

    if (c == '\b')
    {
        if (col)
            col--;
        else if (!col && row)
        {
            row--;
            col = width - 1;
        }
        return;
    }

    ptty_put_entry(c, color, col, row);
    ptty_render_entry(col, row);

    if (col + 1 >= width)
    {
        if (row + 1 >= height)
        {
            tty_shift(1);
            col = 0;
        }
        else
        {
            row++;
            col = 0;
        }
    }
    else
        col++;

    return;
}

#define check_bpp4_color(farbe)          \
	if (strcmp(#farbe, color_code)) \
		color = farbe

void tty_puts(char *data)
{
	unsigned char color = white;
	char color_code[15];

	for (; *data != '\0'; data++)
	{
		if (*data == '$')
		{
			int i = 0;
			data++;
			while (true)
			{
				if (*data != '!')
				{
					color_code[i] = *data;
					i++;
					data++;
				}
				else
				{
					data++;
					break;
				}
			}
			color_code[i + 1] = 0;
			check_bpp4_color(black);
			check_bpp4_color(blue);
			check_bpp4_color(green);
			check_bpp4_color(cyan);
			check_bpp4_color(red);
			check_bpp4_color(magenta);
			check_bpp4_color(brown);
			check_bpp4_color(light_gray);
			check_bpp4_color(dark_gray);
			check_bpp4_color(light_blue);
			check_bpp4_color(light_green);
			check_bpp4_color(light_cyan);
			check_bpp4_color(light_red);
			check_bpp4_color(light_magenta);
			check_bpp4_color(light_brown);
			check_bpp4_color(white);
		}
		tty_putc(*data, color);
	}
}

void tty_shift(unsigned char lines)
{
	if (lines > row)
		return;

	memset(buffer, 0, width * lines);

	for (unsigned long i = 0; i < lines * width; i++)
	{
		buffer[i] = buffer[i + lines * width];
	}

	row -= lines;
	ptty_rerender();
}

const unsigned long kernel_font[] =
{
	0x0000000000000000,
	0x0000000000000000,
	0x000000FF00000000,
	0x000000FF00FF0000,
	0x1818181818181818,
	0x6C6C6C6C6C6C6C6C,
	0x181818F800000000,
	0x6C6C6CEC0CFC0000,
	0x1818181F00000000,
	0x6C6C6C6F607F0000,
	0x000000F818181818,
	0x000000FC0CEC6C6C,
	0x0000001F18181818,
	0x0000007F606F6C6C,
	0x187E7EFFFF7E7E18,  // circle  0x00187EFFFF7E1800
	0x0081818181818100,  // square
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0008000000000000,//
	0x0000000000000000,//
	0x00180018183C3C18,//!
	0x0000000000121236,//"
	0x006C6CFE6CFE6C6C,//#
	0x00187ED07C16FC30,//$$
	0x0060660C18306606,//%
	0x00DC66B61C36361C,//&
	0x0000000000181818,//'
	0x0030180C0C0C1830,//(
	0x000C18303030180C,//)
	0x0000187E3C7E1800,//*
	0x000018187E181800,//+
	0x0C18180000000000,//,
	0x000000007E000000,//-
	0x0018180000000000,//.
	0x0000060C18306000,///
	0x003C42464A52623C,//0
	0x007E101010101C10,//1
	0x007E04081020423C,//2
	0x003C42403840423C,//3
	0x0020207E22242830,//4
	0x003C4240403E027E,//5
	0x003C42423E020438,//6
	0x000404081020407E,//7
	0x003C42423C42423C,//8
	0x001C20407C42423C,//9
	0x0018180018180000,//:
	0x0C18180018180000,//;
	0x0030180C060C1830,//<
	0x0000007E007E0000,//=
	0x000C18306030180C,//>
	0x001800181830663C,//?
	0x003C06765676663C,//@
	0x0042427E42422418,//A
	0x003E42423E42423E,//B
	0x003C42020202423C,//C
	0x001E22424242221E,//D
	0x007E02023E02027E,//E
	0x000202023E02027E,//F
	0x003C42427202423C,//G
	0x004242427E424242,//H
	0x007C10101010107C,//I
	0x001C22202020207E,//J
	0x004222120E0A1222,//K
	0x007E020202020202,//L
	0x0082828292AAC682,//M
	0x00424262524A4642,//N
	0x003C42424242423C,//O
	0x000202023E42423E,//P
	0x005C22424242423C,//Q
	0x004242423E42423E,//R
	0x003C42403C02423C,//S
	0x001010101010107C,//T
	0x003C424242424242,//U
	0x0018244242424242,//V
	0x0044AAAA92828282,//W
	0x0042422418244242,//X
	0x0010101038444444,//Y
	0x007E04081020407E,//Z
	0x003E02020202023E,//[
	0x00006030180C0600,  /* //\ */
	0x007C40404040407C,  //]
	0x000000000000663C,//^
	0xFF00000000000000,//_
	0x000000000030180C,//`
	0x007C427C403C0000,//a
	0x003E4242423E0202,//b
	0x003C4202423C0000,//c
	0x007C4242427C4040,//d
	0x003C027E423C0000,//e
	0x000404043E040438,//f
	0x3C407C42427C0000,//g
	0x00424242423E0202,//h
	0x003C1010101C0018,//i
	0x0E101010101C0018,//j
	0x0042221E22420200,//k
	0x003C101010101018,//l
	0x00829292AA440000,//m
	0x00424242423E0000,//n
	0x003C4242423C0000,//o
	0x02023E42423E0000,//p
	0xC0407C42427C0000,//q
	0x00020202463A0000,//r
	0x003E403C027C0000,//s
	0x00380404043E0404,//t
	0x003C424242420000,//u
	0x0018244242420000,//v
	0x006C929292820000,//w
	0x0042241824420000,//x
	0x3C407C4242420000,//y
	0x007E0418207E0000,//z
	0x003018180E181830,//{
	0x0018181818181818,//|
	0x000C18187018180C,//}
	0x000000000062D68C,//~
	0xFFFFFFFFFFFFFFFF,
	0x1E30181E3303331E,//€
	0x007E333333003300,//
	0x001E033F331E0038,//‚
	0x00FC667C603CC37E,//ƒ
	0x007E333E301E0033,//„
	0x007E333E301E0007,//…
	0x007E333E301E0C0C,//†
	0x3C603E03033E0000,//‡
	0x003C067E663CC37E,//ˆ
	0x001E033F331E0033,//‰
	0x001E033F331E0007,//Š
	0x001E0C0C0C0E0033,//‹
	0x003C1818181C633E,//Œ
	0x001E0C0C0C0E0007,//
	0x00333F33331E0C33,//Ž
	0x00333F331E000C0C,//
	0x003F061E063F0038,//
	0x00FE33FE30FE0000,//‘
	0x007333337F33367C,//’
	0x001E33331E00331E,//“
	0x001E33331E003300,//”
	0x001E33331E000700,//•
	0x007E33333300331E,//–
	0x007E333333000700,//—
	0x1F303F3333003300,//˜
	0x001C3E63633E1C63,//™
	0x001E333333330033,//š
	0x18187E03037E1818,//›
	0x003F67060F26361C,//œ
	0x000C3F0C3F1E3333,//
	0x70337B332F1B1B0F,//ž
	0x0E1B18187E18D870,//Ÿ
	0x007E333E301E0038,//
	0x001E0C0C0C0E001C,//¡
	0x001E33331E003800,//¢
	0x007E333333003800,//£
	0x003333331F001F00,//¤
	0x00333B3F3733003F,//¥
	0x00007E007C36363C,//¦
	0x00007E003C66663C,//§
	0x001E3303060C000C,//¨
	0x000003033F000000,//©
	0x000030303F000000,//ª
	0xF81973C67C1B3363,//«
	0xC0F9F3E6CF1B3363,//¬
	0x183C3C1818001800,//­
	0x0000CC663366CC00,//®
	0x00003366CC663300,//¯
	0x1144114411441144,//°
	0x55AA55AA55AA55AA,//±
	0xEEBBEEBBEEBBEEBB,//²
	0x1818181818181818,//³
	0x1818181F18181818,//´
	0x1818181F181F1818,//µ
	0x6C6C6C6F6C6C6C6C,//¶
	0x6C6C6C7F00000000,//·
	0x1818181F181F0000,//¸
	0x6C6C6C6F606F6C6C,//¹
	0x6C6C6C6C6C6C6C6C,//º
	0x6C6C6C6F607F0000,//»
	0x0000007F606F6C6C,//¼
	0x0000007F6C6C6C6C,//½
	0x0000001F181F1818,//¾
	0x1818181F00000000,//¿
	0x000000F818181818,//À
	0x000000FF18181818,//Á
	0x181818FF00000000,//Â
	0x181818F818181818,//Ã
	0x000000FF00000000,//Ä
	0x181818FF18181818,//Å
	0x181818F818F81818,//Æ
	0x6C6C6CEC6C6C6C6C,//Ç
	0x000000FC0CEC6C6C,//È
	0x6C6C6CEC0CFC0000,//É
	0x000000FF00EF6C6C,//Ê
	0x6C6C6CEF00FF0000,//Ë
	0x6C6C6CEC0CEC6C6C,//Ì
	0x000000FF00FF0000,//Í
	0x6C6C6CEF00EF6C6C,//Î
	0x000000FF00FF1818,//Ï
	0x000000FF6C6C6C6C,//Ð
	0x181818FF00FF0000,//Ñ
	0x6C6C6CFF00000000,//Ò
	0x000000FC6C6C6C6C,//Ó
	0x000000F818F81818,//Ô
	0x181818F818F80000,//Õ
	0x6C6C6CFC00000000,//Ö
	0x6C6C6CEF6C6C6C6C,//×
	0x181818FF00FF1818,//Ø
	0x0000001F18181818,//Ù
	0x181818F800000000,//Ú
	0xFFFFFFFFFFFFFFFF,//Û
	0xFFFFFFFF00000000,//Ü
	0x0F0F0F0F0F0F0F0F,//Ý
	0xF0F0F0F0F0F0F0F0,//Þ
	0x00000000FFFFFFFF,//ß
	0x006E3B133B6E0000,//à
	0x03031F331F331E00,//á
	0x0003030303637F00,//â
	0x0036363636367F00,//ã
	0x007F660C180C667F,//ä
	0x001E3333337E0000,//å
	0x03063E6666666600,//æ
	0x00181818183B6E00,//ç
	0x3F0C1E33331E0C3F,//è
	0x001C36637F63361C,//é
	0x007736366363361C,//ê
	0x001E33333E180C38,//ë
	0x00007EDBDB7E0000,//ì
	0x03067EDBDB7E3060,//í
	0x003C06033F03063C,//î
	0x003333333333331E,//ï
	0x00003F003F003F00,//ð
	0x003F000C0C3F0C0C,//ñ
	0x003F00060C180C06,//ò
	0x003F00180C060C18,//ó
	0x1818181818D8D870,//ô
	0x0E1B1B1818181818,//õ
	0x000C0C003F000C0C,//ö
	0x0000394E00394E00,//÷
	0x000000001C36361C,//ø
	0x0000001818000000,//ù
	0x0000001800000000,//ú
	0x383C3637303030F0,//û
	0x000000363636361E,//ü
	0x0000003E061C301E,//ý
	0x00003C3C3C3C0000,//þ
	0xFFFFFFFFFFFFFFFF //ÿ
};