#pragma once

#include "kernel/io.hxx"

namespace io {
    namespace pit {
        void pit_init();
        void set_frequency(uint32_t hz);
        extern uint32_t frequency_hz;
        extern uint16_t divisor;
    }
}

namespace sys {
    namespace chrono {
        extern "C" volatile uint64_t ms_clock;
        extern "C" void sleep(volatile uint32_t ms);
    }
}