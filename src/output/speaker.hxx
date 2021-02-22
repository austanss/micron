#pragma once

#include "io/io.hxx"
#include "chrono/timer.hxx"

namespace sys {
    namespace audio {
        namespace pcspk {
            void init();
            void play_sound(uint32 frequency);
            void stop_sound();
            void beep();
            void beep(uint32 frequency, uint32 duration);
        }
    }
}