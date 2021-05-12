#pragma once

void tty_initialize();
void tty_clear();
void tty_seek(unsigned short x, unsigned short y);
void tty_putc(char c, unsigned char color);
void tty_puts(char* s);
void tty_shift(unsigned char lines);

enum ground_color : unsigned char {
    black,
    blue,
    green,
    cyan,
    red,
    magenta,
    brown,
    light_gray,
    dark_gray,
    light_blue,
    light_green,
    light_cyan,
    light_red,
    light_magenta,
    light_brown,
    white
};

inline void print(const char* s)
{
    tty_puts((char *)s);
}