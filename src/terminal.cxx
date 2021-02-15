#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>
#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include "kernel/gfx.hxx"

// This terminal emulates a dynamically-sized text mode

uint16_t* terminal::text_buffer;

#define check_vga_color(farbe)          \
	if (util::strcomp(#farbe, colorCode)) \
		color = VGA_COLOR_##farbe;

terminal::terminal() : row(0), column(0)
{
	gfx::shapes::dimensions size = get_optimal_size(gfx::shapes::dims(gfx::gop.x_resolution, gfx::gop.y_resolution));

	VGA_WIDTH = size.w;
	VGA_HEIGHT = size.h;

	text_buffer = (uint16_t *)memory::paging::allocation::request_page();

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

	memory::operations::memset((void *)text_buffer, 0, size.h * size.w * 2);
	memory::operations::memset((void *)gfx::buffer, 0, gfx::gop.framebuffer_size);
	memory::operations::memset((void *)gfx::gop.framebuffer_base, 0, gfx::gop.framebuffer_size);

	io::serial::serial_msg("text buffer at ");	
	io::serial::serial_msg(util::itoa((uint64_t)text_buffer, 16));
}

void terminal::clear()
{
	for (unsigned int y = 0; y < VGA_HEIGHT; y++)
		for (unsigned int x = 0; x < VGA_WIDTH; x++)
			put_entry_at(' ', 0x0, x, y);
		
	setCursor(0, 0);
}

void terminal::put_entry_at(char c, uint8_t vga_color, size_t xpos, size_t ypos)
{
	if (xpos > VGA_WIDTH)
		return;

	if (ypos > VGA_HEIGHT)
		return;

	text_buffer[(ypos * VGA_WIDTH + xpos)] = vga_entry(c, vga_color);

	render_entry_at(xpos, ypos);
}

uint32_t terminal::convert_vga_to_pix(uint8_t vga_color)
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

void terminal::render_entry_at(uint16_t xpos, uint16_t ypos)
{
  	uint32_t fg_color;
  	uint32_t bg_color;

	uint16_t entry = text_buffer[(ypos * VGA_WIDTH + xpos)];

	uint8_t vga_color_fg = (entry & 0x0f00) >> 8;	
	uint8_t vga_color_bg = (entry >> 12);
	
	char c = entry ^ 0b1111111100000000;

	fg_color = convert_vga_to_pix(vga_color_fg);
	bg_color = convert_vga_to_pix(vga_color_bg);

    uint64_t font_selector = gfx::fonts::get_character_font(c);

    uint8_t bits[64];

	for (uint8_t i = 1; i <= 64; i++) {
		bits[i - 1] = util::get_bit(font_selector, i);
	}

		//	for (int i = 63; i >= 0; i--)
		//	{
		//		if ((i + 1) % 8 == 0)
		//			serial_msg('\n');
		//		serial_msg(new_bits[i] + 48); // 48, ASCII code '0'
		//	}

	for (uint32_t y = 0, yy = (ypos * 9); y < 8; y++, yy++) {
		for (uint32_t x = 0, xx = (xpos * 8); x < 8; x++, xx++) {
			if (bits[(8 * y) + x]) {
				gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), fg_color);
			} else {
				gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), bg_color);
			}
		}
	}
}

void terminal::render_entry_at_buffer(uint16_t xpos, uint16_t ypos)
{
  	uint32_t fg_color;
  	uint32_t bg_color;

	uint16_t entry = text_buffer[(ypos * VGA_WIDTH + xpos)];

	uint8_t vga_color_fg = (entry & 0x0f00) >> 8;	
	uint8_t vga_color_bg = (entry >> 12);
	
	char c = entry ^ 0b1111111100000000;

	fg_color = convert_vga_to_pix(vga_color_fg);
	bg_color = convert_vga_to_pix(vga_color_bg);

    uint64_t font_selector = gfx::fonts::get_character_font(c);

    uint8_t bits[64];

	for (uint8_t i = 1; i <= 64; i++) {
		bits[i - 1] = util::get_bit(font_selector, i);
	}

		//	for (int i = 63; i >= 0; i--)
		//	{
		//		if ((i + 1) % 8 == 0)
		//			serial_msg('\n');
		//		serial_msg(new_bits[i] + 48); // 48, ASCII code '0'
		//	}

	for (uint32_t y = 0, yy = (ypos * 9); y < 8; y++, yy+=2) {
		for (uint32_t x = 0, xx = (xpos * 8); x < 8; x++, xx+=2) {
			if (bits[(8 * y) + x]) {
				gfx::screen::plot_pixel_buffer(gfx::shapes::pos(xx, yy), fg_color);
			} else {
				gfx::screen::plot_pixel_buffer(gfx::shapes::pos(xx, yy), bg_color);
			}
		}
	}
}

void terminal::render_buffer()
{
	for (uint16_t ypos = 0; ypos < VGA_HEIGHT; ypos++)
	{
		for (uint16_t xpos = 0; xpos < VGA_WIDTH; xpos++)
		{
			render_entry_at_buffer(xpos, ypos);
		}
	}

	gfx::screen::buff();
}

void terminal::shift()
{
    for (size_t i = 0; i < VGA_WIDTH; i++)
    {
        text_buffer[i] = vga_entry(0, 0);
    }

    for (size_t i = 0; i < VGA_WIDTH; i++)
    {
        for (size_t i = 0; i < (VGA_HEIGHT * VGA_WIDTH); i++) {
            text_buffer[i] = text_buffer[i + 1]; // shift text gfx::buffer to the left
        }
    }
	row--;

	render_buffer();
}

void terminal::put_char(char c, uint8_t color)
{
	if (c == 0)
		return;

	if (c == '\t')
	{
		column = (column / 4 + 1) * 4;
		return;
	}

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

void terminal::write(const char *data, size_t size)
{
	uint8_t color = VGA_COLOR_LIGHT_GREY;
	char colorCode[15];

	size = util::strlen(data);

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
			check_vga_color(BLACK)
			check_vga_color(BLUE)
			check_vga_color(GREEN)
			check_vga_color(CYAN)
			check_vga_color(RED)
			check_vga_color(MAGENTA)
			check_vga_color(BROWN)
			check_vga_color(LIGHT_GREY)
			check_vga_color(DARK_GREY)
			check_vga_color(LIGHT_BLUE)
			check_vga_color(LIGHT_GREEN)
			check_vga_color(LIGHT_CYAN)
			check_vga_color(LIGHT_RED)
			check_vga_color(LIGHT_MAGENTA)
			check_vga_color(LIGHT_BROWN)
			check_vga_color(WHITE)
		}
		put_char(*data, color);
	}
}
void terminal::write(const char *data)
{
	if (util::strcomp(data, terminal_line))
	{
		put_char('\n', 0x000000);
		for (size_t i = 1; i <= VGA_WIDTH; i++)
		{
			if (i % 2 == 0)
				put_char('=', 15);
			else
				put_char('-', 15);
		}
		put_char('\n', 0x000000);
		return;
	}

	write(data, util::strlen(data));
}

void terminal::setCursor(size_t columnc, size_t rowc)
{
	column = columnc;
	row = rowc;
}

void terminal::write(int data)
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
void terminal::println(const char *data)
{
	write(data);
	write("\n");
}

terminal& terminal::instance()
{
	static terminal instance;
	return instance;
}

gfx::shapes::dimensions terminal::get_optimal_size(gfx::shapes::dimensions screen_res) {
	gfx::shapes::dimensions term_size;

	term_size.w = (screen_res.w / 8);
	term_size.h = (screen_res.h / 9);

	return term_size;
}

extern "C" void puts(char* data)
{
	printf("%s\n", data);
}