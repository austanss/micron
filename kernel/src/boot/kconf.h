#pragma once
#include <stddef.h>
#include "types.h"
#include "symbols.h"
#include "boot/boot.h"
#include "drivers/acpi/acpi.h"

namespace sys {
    namespace config {
        extern "C" uint64 __kernel_start;
        extern "C" uint64 __kernel_end;
        extern "C" __attribute__((weak)) const symbol __kernel_symtab[];
        extern uint64 __kernel_size;
        extern uint64 __kernel_pages;
        void calculate_kernel_size();
        void setup_paging(stivale2_struct_tag_framebuffer *framebuffer);
        void configure_memory(stivale2_struct_tag_framebuffer *framebuffer, stivale2_struct_tag_memmap *memory_map);
        void configure_pci(sys::acpi::rsdp2* rsdp);
    }
}