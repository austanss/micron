#pragma once
#include "types.hxx"

#define PIC1		    0x20
#define PIC2		    0xA0
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	    (PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	    (PIC2+1)
#define PIC_EOI		    0x20

#define ICW1_ICW4	    0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	    0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	    0x10		/* Initialization - required! */

#define ICW4_8086	    0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	    0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */

namespace io {

    static inline void outb(word port, byte val)
    {
        asm volatile("outb %0, %1"
                    :
                    : "a"(val), "Nd"(port));
    }

    static inline byte inb(word port)
    {
        byte ret;
        asm volatile("inb %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }

    static inline void outw(word port, byte val)
    {
        asm volatile("outw %0, %1"
                    :
                    : "a"(val), "Nd"(port));
    }

    static inline word inw(word port)
    {
        word ret;
        asm volatile("inw %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }


    static inline void outl(word port, long val)
    {
        asm volatile("outl %0, %1"
                    :
                    : "a"(val), "Nd"(port));
    }

    static inline qword inl(word port)
    {
        word ret;
        asm volatile("inl %1, %0"
                    : "=a"(ret)
                    : "Nd"(port));
        return ret;
    }

    static inline void io_wait()
    {
        asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    }


    namespace serial {
        extern "C" void     serial_msg(const char *val);
        static inline void  serial_byte(byte val) {
            io::outb(0x3F8, val);
        }
    }

    namespace pic {
        extern "C" void     irq_mask(unsigned char irq);
        extern "C" void     irq_unmask(unsigned char irq);
        extern "C" void     pic_remap(byte offset1, byte offset2);
        extern "C" void     pic_send_eoi(unsigned char irq);
    }
}