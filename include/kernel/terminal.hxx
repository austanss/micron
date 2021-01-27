#pragma once

#include "stddef.h"
#include "stdint.h"
#include "gfx.hxx"

    #define status_pend "$WHITE![$LIGHT_BLUE!-$WHITE!] $LIGHT_GREY!\0"
    #define status_good "$WHITE![$LIGHT_GREEN!+$WHITE!] $LIGHT_GREY!\0"
    #define status_fail "$WHITE![$RED!X$WHITE!] $LIGHT_GREY!\0"
    #define status_eol "\n\0"
	#define terminal_line "$TERM_LINE!\0"

class Terminal
{

public:
	size_t row;
	size_t column;
    static Terminal &instance();
    void put_entry_at(char c, uint8_t vga_color, size_t x, size_t y);
    void put_char(char c, uint8_t color);
    void write(const char* data, size_t size);
    void write(const char* data);
    void write(int num);
    void println(const char* data = "");
    void shift();
	void clear();
	void setCursor(size_t columnc, size_t rowc);
	static dimensions get_optimal_size(dimensions screen_res);
	bool staticLogo = false;
    void render_buffer();
    void render_entry_at(uint16_t xpos, uint16_t ypos);

private:
	Terminal();
	Terminal(Terminal const&);
	size_t VGA_WIDTH = 64;
	size_t VGA_HEIGHT = 30;
	void operator=(Terminal const&);
};

template<typename T>
Terminal& operator<<(Terminal& term, T data)
{
        term.write(data);
        return term;
}

template<typename T>
Terminal* operator<<(Terminal* term, T data)
{
        term->write(data);
        return term;
}

extern "C" void puts(char* data);