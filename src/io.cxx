#include "kernel/io.hxx"
#include "kernel/pic.hxx"
#include "kernel/string.hxx"

void outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

void IRQ_set_mask(unsigned char IRQLine) {
    uint16_t port;
    uint8_t value;
 
    if(IRQLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = inb(port) | (1 << IRQLine);
    outb(port, value);        
}
 
void IRQ_clear_mask(unsigned char IRQLine) {
    uint16_t port;
    uint8_t value;
 
    if(IRQLine < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQLine -= 8;
    }
    value = inb(port) & ~(1 << IRQLine);
    outb(port, value);        
}

void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
}

extern "C" void serial_byte(uint8_t val) {
	outb(0x3F8, val);
}

extern "C" void serial_msg(const char *val) {
	for (size_t i = 0; i < strlen(val); i++) {
		serial_byte(val[i]);
	}
}
