#pragma once
#include <stdint.h>

void tty_initialize();
void tty_putc(char c, uint32_t color);
void tty_puts(char* s);
void tty_shift(uint8_t lines);
void tty_clear();
void tty_seek(uint16_t x, uint16_t y);