#pragma once
#include <stddef.h>
#include <stdint.h>
#include "kernel/boot.hxx"

namespace sys {
    namespace config {
        extern "C" uint64_t _kernel_start;
        extern "C" uint64_t _kernel_end;
        extern uint64_t _kernel_size;
        extern uint64_t _kernel_pages;
        void calculate_kernel_size();
        void setup_paging(stivale_framebuffer *framebuffer);
        void configure_memory(stivale_framebuffer *framebuffer, stivale_memory_map *memory_map);
        void configure_graphics(stivale_framebuffer *framebuffer);
        void configure_userspace();
    }
}