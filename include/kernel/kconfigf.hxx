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
        void setup_paging(boot::boot_info* bootloader_info);
        void configure_memory(boot::boot_info* bootloader_info);
        void configure_graphics(boot::boot_info* bootloader_info);
        void boot_info_copy(boot::boot_info* dst, boot::boot_info* src);
    }
}