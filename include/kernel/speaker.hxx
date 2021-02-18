#pragma once

#include "kernel/io.hxx"
#include "kernel/timer.hxx"

namespace sys {
    namespace audio {
        namespace pcspk {
            void init();
            void play_sound(uint32_t frequency);
            void stop_sound();
            void beep();
            void beep(uint32_t frequency, uint32_t duration);
        };
    };
}