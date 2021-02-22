#pragma once

#include "types.hxx"

#define CONFIG_MAX_CPUS 1

namespace cpu {
    namespace tss {
        typedef struct tss {
            dword   prev_tss;
            address rsp[3];
            address reserved0;
            address ist[7];
            address reserved1;
            word    reserved2;
            word    io_map;
        } __attribute__((packed)) tss_t;

        void tss_init();
        extern "C" void tss_install(int num_cpu, address cpu_stack);

        void tss_setstack(int num_cpu, address stack);

        extern "C" void   load_tss(word);
        extern "C" tss_t* tss_get(int num_cpu);
    }
}