#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/string.hxx"
#include "kernel/io.hxx"
#include "kernel/logo.hxx"
#include "kernel/uart.hxx"
#include "kernel/kutil.hxx"
#include "kernel/gfx.hxx"
#include "kernel/macros.hxx"
#include <cstdint>

// This terminal emulates a dynamically-sized text mode

uint16_t* text_buffer;

enum vga_color
{
    VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15
};

#define CHECK_COLOR(farbe)          \
	if (strcomp(#farbe, colorCode)) \
		color = VGA_COLOR_##farbe;

Terminal::Terminal() : row(0), column(0)
{
	dimensions size = get_optimal_size(dims(gop.x_resolution, gop.y_resolution));

	VGA_WIDTH = size.w;
	VGA_HEIGHT = size.h;

	for (unsigned int x = 0; x <= gop.x_resolution; x++)
		for (unsigned int y = 0; y <= gop.y_resolution; y++)
			buffer[y * gop.x_resolution + x] = 0x00000000;

	buff();

	text_buffer = (uint16_t *)kmalloc((size.h * size.w) * 2 + 8);

	uint8_t* text = (uint8_t *)text_buffer;
	text[0] = 'T';
	text[1] = 'E';
	text[2] = 'X';
	text[3] = 'T';
	text[4] = 'B';
	text[5] = 'U';
	text[6] = 'F';
	text[7] = 'F';

	text_buffer += 8;
}

void Terminal::clear()
{
	for (unsigned int y = 0; y < VGA_HEIGHT; y++)
		for (unsigned int x = 0; x < VGA_WIDTH; x++)
			put_entry_at(' ', 0x0, x, y);
		
	setCursor(0, 0);
}

void Terminal::put_entry_at(char c, uint8_t vga_color, size_t xpos, size_t ypos)
{
	if (xpos > VGA_WIDTH)
		return;

	if (ypos > VGA_HEIGHT)
		return;

	text_buffer[(ypos * VGA_WIDTH + xpos)] = vga_entry(c, vga_color);

	render_entry_at(xpos, ypos);
}

uint32_t Terminal::convert_vga_to_pix(uint8_t vga_color)
{
	uint32_t color;
	switch (vga_color) 
	{
		case VGA_COLOR_BLACK:
			color = 0x000000;
			break;

		case VGA_COLOR_WHITE:
			color = 0xFFFFFF;
			break;

		case VGA_COLOR_RED:
			color = 0xFF0000;
			break;

		case VGA_COLOR_BLUE:
			color = 0x2222FF;
			break;

		case VGA_COLOR_GREEN:
			color = 0x22FF22;
			break;

		case VGA_COLOR_CYAN:
			color = 0x11FFFF;
			break;

		case VGA_COLOR_MAGENTA:
			color = 0xFF01AA;
			break;

		case VGA_COLOR_BROWN:
			color = 0xFFEBCD;
			break;

		case VGA_COLOR_LIGHT_GREY:
			color = 0xDDDDDD;
			break;

		case VGA_COLOR_DARK_GREY:
			color = 0x555555;
			break;

		case VGA_COLOR_LIGHT_BLUE:
			color = 0x01AAFF;
			break;

		case VGA_COLOR_LIGHT_GREEN:
			color = 0x01FF01;
			break;

		case VGA_COLOR_LIGHT_CYAN:
			color = 0x01DDFF;
			break;

		case VGA_COLOR_LIGHT_RED:
			color = 0xFF2222;
			break;

		case VGA_COLOR_LIGHT_MAGENTA:
			color = 0xFF0077;
			break;

		case VGA_COLOR_LIGHT_BROWN:
			color = 0x8B4513;
			break;
	}

	return color;
}

void Terminal::render_entry_at(uint16_t xpos, uint16_t ypos)
{
  	uint32_t color;

	uint16_t entry = text_buffer[(ypos * VGA_WIDTH + xpos)];

	uint8_t vga_color = (entry >> 8) ^ 0b1111111100000000;
	char c = entry ^ 0b1111111100000000;

	color = convert_vga_to_pix(vga_color);

    uint64_t font_selector = FONT[c];

    uint8_t bits[64];

	for (uint8_t i = 1; i <= 64; i++) {
		bits[i] = get_bit(font_selector, i);
	}

		//	for (int i = 63; i >= 0; i--)
		//	{
		//		if ((i + 1) % 8 == 0)
		//			serial_msg('\n');
		//		serial_msg(new_bits[i] + 48); // 48, ASCII code '0'
		//	}

	for (uint32_t y = 0, yy = (ypos * 25); y < 8; y++, yy += 3) {
		for (uint32_t x = 0, xx = (xpos * 16); x < 8; x++, xx += 2) {
			if (bits[(8 * y) + x]) {
				plot_pixel(pos(xx, yy), color);
				plot_pixel(pos(xx + 1, yy), color);
				plot_pixel(pos(xx, yy + 1), color);
				plot_pixel(pos(xx + 1, yy + 1), color);
				plot_pixel(pos(xx, yy + 2), color);
				plot_pixel(pos(xx + 1, yy + 2), color);
			} else {
				plot_pixel(pos(xx, yy), 0x00000000);
				plot_pixel(pos(xx + 1, yy), 0x00000000);
				plot_pixel(pos(xx, yy + 1), 0x00000000);
				plot_pixel(pos(xx + 1, yy + 1), 0x00000000);
				plot_pixel(pos(xx, yy + 2), 0x00000000);
				plot_pixel(pos(xx + 1, yy + 2), 0x00000000);
			}
		}
	}
}

void Terminal::render_buffer()
{
	for (uint16_t ypos = 0; ypos < VGA_HEIGHT; ypos++)
	{
		for (uint16_t xpos = 0; xpos < VGA_WIDTH; xpos++)
		{
			uint32_t color;

			uint16_t entry = text_buffer[(ypos * VGA_WIDTH + xpos)];

			uint8_t vga_color = (entry >> 8) ^ 0b1111111100000000;
			char c = entry ^ 0b1111111100000000;

			color = convert_vga_to_pix(vga_color);

			uint64_t font_selector = FONT[c];

			uint8_t bits[64];

			for (uint8_t i = 1; i <= 64; i++) {
				bits[i] = get_bit(font_selector, i);
			}

				//	for (int i = 63; i >= 0; i--)
				//	{
				//		if ((i + 1) % 8 == 0)
				//			serial_msg('\n');
				//		serial_msg(new_bits[i] + 48); // 48, ASCII code '0'
				//	}

			for (uint32_t y = 0, yy = (ypos * 25); y < 8; y++, yy += 3) {
				for (uint32_t x = 0, xx = (xpos * 16); x < 8; x++, xx += 2) {
					if (bits[(8 * y) + x]) {
						plot_pixel(pos(xx, yy), color);
						plot_pixel(pos(xx + 1, yy), color);
						plot_pixel(pos(xx, yy + 1), color);
						plot_pixel(pos(xx + 1, yy + 1), color);
						plot_pixel(pos(xx, yy + 2), color);
						plot_pixel(pos(xx + 1, yy + 2), color);
					} else {
						plot_pixel(pos(xx, yy), 0x00000000);
						plot_pixel(pos(xx + 1, yy), 0x00000000);
						plot_pixel(pos(xx, yy + 1), 0x00000000);
						plot_pixel(pos(xx + 1, yy + 1), 0x00000000);
						plot_pixel(pos(xx, yy + 2), 0x00000000);
						plot_pixel(pos(xx + 1, yy + 2), 0x00000000);
					}
				}
			}
		}
	}
}

void Terminal::shift()
{
    for (int i = 0; i < VGA_WIDTH; i++)
    {
        text_buffer[i] = vga_entry(0, 0);
    }

    DO(VGA_WIDTH) // shift VGA_WIDTH times
    {
        for (int i = 0; i < (VGA_HEIGHT * VGA_WIDTH); i++) {
            text_buffer[i] = text_buffer[i + 1]; // shift text buffer to the left
        }
    }
	row--;

	render_buffer();
}

void Terminal::put_char(char c, uint8_t color)
{
	if (c == 0)
		return;

	if (c == '\b')
	{
		if (--column < 0)
			column = (VGA_WIDTH - 1);

		put_entry_at(' ', 0x0, column, row);
		return;
	}

	if (c != '\n')
		put_entry_at(c, color, column, row);

	if (++column >= VGA_WIDTH)
	{
		column = 0;
		if (++row >= VGA_HEIGHT)
		{
			shift();
		}
	}
	if (c == '\n')
	{
		if (++row >= VGA_HEIGHT)
		{
			shift();
		}
		column = 0;
	}
}

void Terminal::write(const char *data, size_t size)
{
	uint8_t color = VGA_COLOR_LIGHT_GREY;
	char colorCode[15];

	for (; *data != '\0'; data++)
	{
		if (*data == '$')
		{
			int iter = 0;
			data++;
			while (true)
			{
				if (*data != '!')
				{
					colorCode[iter] = *data;
					iter++;
					data++;
				}
				else
				{
					data++;
					break;
				}
			}
			colorCode[iter + 1] = 0;
			CHECK_COLOR(BLACK)
			CHECK_COLOR(BLUE)
			CHECK_COLOR(GREEN)
			CHECK_COLOR(CYAN)
			CHECK_COLOR(RED)
			CHECK_COLOR(MAGENTA)
			CHECK_COLOR(BROWN)
			CHECK_COLOR(LIGHT_GREY)
			CHECK_COLOR(DARK_GREY)
			CHECK_COLOR(LIGHT_BLUE)
			CHECK_COLOR(LIGHT_GREEN)
			CHECK_COLOR(LIGHT_CYAN)
			CHECK_COLOR(LIGHT_RED)
			CHECK_COLOR(LIGHT_MAGENTA)
			CHECK_COLOR(LIGHT_BROWN)
			CHECK_COLOR(WHITE)
		}
		put_char(*data, color);
	}
}
void Terminal::write(const char *data)
{
	if (strcomp(data, terminal_line))
	{
		put_char('\n', 0x000000);
		for (int i = 1; i <= VGA_WIDTH; i++)
		{
			if (i % 2 == 0)
				put_char('=', 0xFFFFFF);
			else
				put_char('-', 0xFFFFFF);
		}
		put_char('\n', 0x000000);
		return;
	}

	write(data, strlen(data));
}

void Terminal::setCursor(size_t columnc, size_t rowc)
{
	column = columnc;
	row = rowc;
}

void Terminal::write(int data)
{
	auto convert = [](unsigned int num)
	{
		static char numberCharacters[]= "0123456789";
		static char buffer[64];
		char *ptr;

		ptr = &buffer[49];
		*ptr = '\0';

		do {
			*--ptr = numberCharacters[num % 10];
			num /= 10;
		} while(num != 0);

		return ptr;
	};

	if (data < 0) {
		write("-");
	}
	write(convert(data));
}
void Terminal::println(const char *data)
{
	write(data);
	write("\n");
}

Terminal& Terminal::instance()
{
	static Terminal instance;
	return instance;
}

dimensions Terminal::get_optimal_size(dimensions screen_res) {
	dimensions term_size;

	term_size.w = (screen_res.w / 16);
	term_size.h = (screen_res.h / 25);

	return term_size;
}

extern "C" void puts(char* data)
{
	Terminal::instance().write(data);
}