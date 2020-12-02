#pragma once

#include <stdint.h>

void outb(uint16_t port, uint8_t val);

uint8_t inb(uint16_t port);

void irq_mask(unsigned char IRQLine);

void irq_unmask(unsigned char IRQLine);

void io_wait(void);

extern "C" void serial_msg(const char *val);
extern "C" void serial_byte(uint8_t val);