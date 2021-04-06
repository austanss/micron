#pragma once
#include <stddef.h>
#include "types.hxx"
#include "start/boot.hxx"
#include "io/acpi.hxx"

namespace sys {
    namespace config {
        extern "C" uint64 __kernel_start;
        extern "C" uint64 __kernel_end;
        extern uint64 __kernel_size;
        extern uint64 __kernel_pages;
        void calculate_kernel_size();
        void setup_paging(stivale_framebuffer *framebuffer);
        void configure_memory(stivale_framebuffer *framebuffer, stivale_memory_map *memory_map);
        void configure_graphics(stivale_framebuffer *framebuffer);
        void configure_pci(sys::acpi::rsdp2* rsdp);
        void configure_userspace();
    }
}