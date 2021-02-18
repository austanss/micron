#pragma once

#include "kernel/io.hxx"

namespace io {
    namespace pit {
        void pit_init();
        void set_c0_frequency(uint32_t hz);
        void set_c2_frequency(uint32_t hz);
        extern uint32_t frequency_hz;
        extern uint16_t divisor;
        extern uint32_t c2_frequency_hz;
        extern uint16_t c2_divisor;
    }
}

namespace sys {
    namespace chrono {
        extern "C" volatile uint64_t ms_clock;
        extern "C" void sleep(volatile uint32_t ms);
    }
}