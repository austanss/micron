#pragma once
#include <stddef.h>
#include "types.hxx"
#include "start/boot.hxx"
#include "io/acpi.hxx"

namespace sys {
    namespace config {
        extern "C" address _kernel_start;
        extern "C" address _kernel_end;
        extern uint64 _kernel_size;
        extern uint64 _kernel_pages;
        void calculate_kernel_size();
        void setup_paging(stivale_framebuffer *framebuffer);
        void configure_memory(stivale_framebuffer *framebuffer, stivale_memory_map *memory_map);
        void configure_graphics(stivale_framebuffer *framebuffer);
        void configure_pci(sys::acpi::rsdp2* rsdp);
        void configure_userspace();
    }
}