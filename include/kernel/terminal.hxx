#pragma once

#include "stddef.h"
#include "stdint.h"
#include "gfx.hxx"

    #define status_pend "$WHITE![$LIGHT_BLUE!-$WHITE!] $LIGHT_GREY!\0"
    #define status_good "$WHITE![$LIGHT_GREEN!+$WHITE!] $LIGHT_GREY!\0"
    #define status_fail "$WHITE![$RED!X$WHITE!] $LIGHT_GREY!\0"
    #define status_eol "\n\0"
	#define terminal_line "$TERM_LINE!\0"

class terminal
{

public:
    static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
    {
    	return (uint16_t)uc | (uint16_t)color << 8;
    }

    static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) 
    {
	    return fg | bg << 4;
    }

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
	size_t row;
	size_t column;
    static terminal &instance();
    void put_entry_at(char c, uint8_t vga_color, size_t x, size_t y);
    void put_char(char c, uint8_t color);
    void write(const char* data, size_t size);
    void write(const char* data);
    void write(int num);
    void println(const char* data = "");
    void shift();
	void clear();
    uint32_t convert_vga_to_pix(uint8_t vga_color);
	void setCursor(size_t columnc, size_t rowc);
	static gfx::shapes::dimensions get_optimal_size(gfx::shapes::dimensions screen_res);
	bool staticLogo = false;
    void render_buffer();
    void render_entry_at(uint16_t xpos, uint16_t ypos);
    void render_entry_at_buffer(uint16_t xpos, uint16_t ypos);
    static uint16_t* text_buffer;

private:
	terminal();
	terminal(terminal const&);
	size_t VGA_WIDTH = 64;
	size_t VGA_HEIGHT = 30;
	void operator=(terminal const&);
};

extern "C" void puts(char* data);
extern "C" int printf(const char* __restrict format, ...);