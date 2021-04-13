#pragma once
#include "types.h"

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
}