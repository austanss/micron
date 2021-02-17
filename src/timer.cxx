#include "kernel/timer.hxx"

uint16_t io::pit::divisor = 0;
uint32_t io::pit::frequency_hz = 0;

void io::pit::pit_init()
{
    io::pic::irq_unmask(0);
}

void io::pit::set_frequency(uint32_t hz)
{
    asm volatile ("cli");
    frequency_hz = hz;
    divisor = 1193182 / hz;             /* Calculate our divisor */
    io::outb(0x43, 0x34);               /* Set our command byte 0x34 */
    io::outb(0x40, divisor & 0xFF);     /* Set low byte of divisor */
    io::outb(0x40, divisor >> 8);       /* Set high byte of divisor */
    asm volatile ("sti");
}

extern "C" volatile uint64_t sys::chrono::ms_clock = 0;