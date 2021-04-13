#pragma once

#include "types.h"

#define CONFIG_MAX_CPUS 1

namespace cpu {
    namespace tss {
        typedef struct tss {
            dword   pad;
            address rsp[3];
            qword   pad_ex;
            address ist[7];
            qword   pad_ex_ex;
            word    pad_ex_ex_ex;
            word    io_map;
        } __attribute__((packed)) tss_t;

        void tss_init();
        extern "C" void tss_install(int num_cpu, address cpu_stack);

        void tss_setstack(int num_cpu, address stack);

        extern "C" void   load_tss(word);
        extern "C" tss_t* tss_get(int num_cpu);
    }
}