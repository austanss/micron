#pragma once
#include "contexts.h"

namespace sys {
    namespace proc {
        process*        create_process(char* name, void* code, uint64 flags);
        process*        grab_process(char* name);
        void            kill_process(process* process);
        thread*         create_thread(process* process, void* code);
        void            kill_thread(thread* thread);
    }
}