#pragma once
#include "../io.h"

namespace io {
    namespace serial {
        extern "C" void     serial_msg(const char *val);
        static inline void  serial_byte(byte val) {
            io::outb(0x3F8, val);    
        }

        namespace console {
            void init();
            void read_character();
        }
    }
}