#include "kernel/io.hxx"
#include "kernel/kutil.hxx"

void io::outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

uint8_t io::inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

void io::outw(uint16_t port, uint16_t val)
{
	asm volatile("outw %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

uint16_t io::inw(uint16_t port)
{
	uint16_t ret;
	asm volatile("inw %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

void io::io_wait(void)
{
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

extern "C" void io::serial::serial_byte(uint8_t val) {
	io::outb(0x3F8, val);
}

extern "C" void io::serial::serial_msg(const char *val) {
	for (size_t i = 0; i < util::strlen(val); i++) {
		serial_byte(val[i]);
	}
}
