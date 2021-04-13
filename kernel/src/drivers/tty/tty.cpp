#include <stdarg.h>
#include <limits.h>
#include "types.h"
#include "tty.h"
#include "memory/pmm.h"
#include "memory/operations.h"
#include "drivers/io.h"
#include "drivers/uart/serial.h"
#include "util/kutil.h"

// This terminal emulates a dynamically-sized text mode


uint 	io::tty::vga_width;
uint 	io::tty::vga_height;
uint 	io::tty::row;
uint 	io::tty::column;
word* 	io::tty::text_buffer;

#define check_vga_color(farbe)          \
	if (util::strcomp(#farbe, colorCode)) \
		color = VGA_COLOR_##farbe;

void io::tty::initialize()
{
	row = 0;
	column = 0;

	gfx::shapes::dimensions size = get_optimal_size(gfx::shapes::dims(gfx::gop.framebuffer_width, gfx::gop.framebuffer_height));

	vga_width = size.w;
	vga_height = size.h;

	text_buffer = (word *)memory::pmm::request_page();

	memory::operations::memset((void *)text_buffer, 0, size.h * size.w * 2);
	memory::operations::memset((void *)gfx::buffer, 0, gfx::gop.framebuffer_width * gfx::gop.framebuffer_height * (gfx::gop.framebuffer_bpp / 8));
	memory::operations::memset((void *)gfx::gop.framebuffer_addr, 0, gfx::gop.framebuffer_width * gfx::gop.framebuffer_height * (gfx::gop.framebuffer_bpp / 8));

	io::serial::serial_msg("text buffer at ");
	io::serial::serial_msg(util::itoa((address)text_buffer, 16));
}

void io::tty::clear()
{
	for (unsigned int y = 0; y < vga_height; y++)
		for (unsigned int x = 0; x < vga_width; x++)
			put_entry_at(' ', 0x0, x, y);
		
	set_cursor(0, 0);
}

void io::tty::put_entry_at(char c, byte vcolor, size_t xpos, size_t ypos)
{
	if (xpos > vga_width)
		return;

	if (ypos > vga_height)
		return;

	text_buffer[(ypos * vga_width + xpos)] = vga_entry(c, vcolor);

	render_entry_at(xpos, ypos);
}

dword io::tty::convert_vga_to_pix(byte vcolor)
{
	dword color = 0;
	switch (vcolor) 
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

void io::tty::render_entry_at(uint16 xpos, uint16 ypos)
{
  	dword fg_color;
  	dword bg_color;

	word entry = text_buffer[(ypos * vga_width + xpos)];

	byte vga_color_fg = (entry & 0x0f00) >> 8;	
	byte vga_color_bg = (entry >> 12);
	
	char c = entry ^ 0b1111111100000000;

	fg_color = convert_vga_to_pix(vga_color_fg);
	bg_color = convert_vga_to_pix(vga_color_bg);

    qword font_selector = gfx::fonts::get_character_font(c);

    byte bits[64];

	for (uint08 i = 1; i <= 64; i++) {
		bits[i - 1] = util::get_bit(font_selector, i);
	}

	for (uint y = 0, yy = (ypos * 9); y < 8; y++, yy++) {
 		for (uint x = 0, xx = (xpos * 8); x < 8; x++, xx++) {
			if (bits[(8 * y) + x]) {
				gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), fg_color);
			} else {
				gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), bg_color);
			}
		}
	}
}

void io::tty::render_entry_at_buffer(uint16 xpos, uint16 ypos)
{
  	dword fg_color;
  	dword bg_color;

	word entry = text_buffer[(ypos * vga_width + xpos)];

	byte vga_color_fg = (entry & 0x0f00) >> 8;	
	byte vga_color_bg = (entry >> 12);
	
	char c = entry ^ 0b1111111100000000;

	fg_color = convert_vga_to_pix(vga_color_fg);
	bg_color = convert_vga_to_pix(vga_color_bg);

    qword font_selector = gfx::fonts::get_character_font(c);

    byte bits[64];

	for (uint08 i = 1; i <= 64; i++) {
		bits[i - 1] = util::get_bit(font_selector, i);
	}

	for (uint y = 0, yy = (ypos * 9); y < 8; y++, yy+=2) {
		for (uint x = 0, xx = (xpos * 8); x < 8; x++, xx+=2) {
			if (bits[(8 * y) + x]) {
				gfx::screen::plot_pixel_buffer(gfx::shapes::pos(xx, yy), fg_color);
			} else {
				gfx::screen::plot_pixel_buffer(gfx::shapes::pos(xx, yy), bg_color);
			}
		}
	}
}

void io::tty::render_buffer()
{
	for (uint16 ypos = 0; ypos < vga_height; ypos++)
	{
		for (uint16 xpos = 0; xpos < vga_width; xpos++)
		{
			render_entry_at(xpos, ypos);
		}
	}

//	gfx::screen::buff();
}

void io::tty::shift()
{
    for (size_t i = 0; i < vga_width; i++)
    {
        text_buffer[i] = vga_entry(0, 0);
    }

    for (size_t i = 0; i < vga_width; i++)
    {
        for (size_t j = 0; j < (vga_height * vga_width); j++) {
            text_buffer[j] = text_buffer[j + 1]; // shift text buffer to the left
        }
    }
	row--;

	render_buffer();
}

void io::tty::put_char(char c, byte color)
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
		if (column == 0)
		{
			if (row != 0)
			{
				column = (vga_width - 1);
				row--;
			}
		}
		else
			column--;

		put_entry_at(' ', 0x0, column, row);
		return;
	}

	if (c != '\n')
		put_entry_at(c, color, column, row);

	if (++column >= vga_width)
	{
		column = 0;
		if (++row >= vga_height)
		{
			shift();
		}
	}
	if (c == '\n')
	{
		if (++row >= vga_height)
		{
			shift();
		}
		column = 0;
	}
}

void io::tty::write(const char *data)
{
	byte color = VGA_COLOR_LIGHT_GREY;
	char colorCode[15];

	if (util::strcomp(data, terminal_line))
	{
		for (uint i = 0; i < vga_width; i++)
		{
			if (!(i % 2))
				put_char('-', 0x0F);
			else
				put_char('=', 0x0F);
		}
		return;
	}

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

void io::tty::set_cursor(size_t columnc, size_t rowc)
{
	column = columnc;
	row = rowc;
}

void io::tty::write_line(const char *data)
{
	write(data);
	write("\n");
}

gfx::shapes::dimensions io::tty::get_optimal_size(gfx::shapes::dimensions screen_res) {
	gfx::shapes::dimensions term_size;

	term_size.w = (screen_res.w / 8);
	term_size.h = (screen_res.h / 9);

	return term_size;
}

extern "C" void puts(char* data)
{
	printf("%s\n", data);
}