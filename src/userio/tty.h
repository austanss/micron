#pragma once

#include <stddef.h>
#include "types.h"
#include "userio/gfx.h"

    #define status_pend "$WHITE![$LIGHT_BLUE!-$WHITE!] $LIGHT_GREY!\0"
    #define status_good "$WHITE![$LIGHT_GREEN!+$WHITE!] $LIGHT_GREY!\0"
    #define status_fail "$WHITE![$RED!X$WHITE!] $LIGHT_GREY!\0"
    #define status_eol "\n\0"
	#define terminal_line "$TERM_LINE!\0"

namespace io {
    namespace tty {
        extern uint vga_width;
        extern uint vga_height;
        extern uint row;
        extern uint column;
        extern word* text_buffer;

        static inline word vga_entry(byte uc, byte color)
        { return (word)uc | (word)color << 8; }
        static inline byte vga_entry_color(byte fg, byte bg) 
        { return fg | bg << 4; }
        void put_entry_at(char c, byte vcolor, size_t x, size_t y);
        void put_char(char c, byte color);
        void write(const char* data);
        void write_line(const char* data = "");
        void shift();
        void clear();
        dword convert_vga_to_pix(byte vcolor);
        void set_cursor(size_t columnc, size_t rowc);
        gfx::shapes::dimensions get_optimal_size(gfx::shapes::dimensions screen_res);
        void render_buffer();
        void render_entry_at(uint16 xpos, uint16 ypos);
        void render_entry_at_buffer(uint16 xpos, uint16 ypos);
        void initialize();

        
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
        
    }
}

extern "C" void puts(char* data);
extern "C" int printf(const char* __restrict format, ...);