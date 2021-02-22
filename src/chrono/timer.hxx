#pragma once

#include "io/io.hxx"

namespace io {
    namespace pit {
        void pit_init();
        void set_c0_frequency(uint32 hz);
        void set_c2_frequency(uint32 hz);
        extern uint32 frequency_hz;
        extern uint16 divisor;
        extern uint32 c2_frequency_hz;
        extern uint16 c2_divisor;
    }
}

namespace sys {
    namespace chrono {
        extern "C" uint ms_clock;
        extern "C" void sleep(uint ms);
    }
}