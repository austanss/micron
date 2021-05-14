#pragma once
#include "types.h"

namespace sys {
    namespace proc {
        struct gpr_state {
            uint64  rax;
            uint64  rbx;
            uint64  rcx;
            uint64  rdx;
            uint64  r08;
            uint64  r09;
            uint64  r10;
            uint64  r11;
            uint64  r12;
            uint64  r13;
            uint64  r14;
            uint64  r15;
        };

        struct page_state {
            uint64  tables;
            uint64  fault;
        };

        struct execution_state {
            uint64  ip;
            uint64  bp;
            uint64  sp;
        }; 

        struct segment_state {
            uint64  code;
            uint64  data;
            uint64  extra;
            uint64  file;
            uint64  general;
        }; 

        struct thread_context {
            gpr_state*          registers;
            page_state*         paging;
            execution_state*    execution;
            segment_state*      segments;
        };

        struct thread {
            uint64          thread_id;
            process*        host;
            thread_context* context;
        };

        struct thread_node {
            thread*         thread;
            thread_node*    next;
        };

        struct process {
            char            id[232];
            thread_node*    threads;
            uint64          thread_count;
            uint64          flags;
        };
    }
}