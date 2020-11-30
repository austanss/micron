#pragma once

#include <stdint.h>

void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);

void IRQ_set_mask(unsigned char IRQLine);

void IRQ_clear_mask(unsigned char IRQLine);

void io_wait(void);

extern "C" void serial_msg(const char *val);
extern "C" void serial_byte(uint8_t val);