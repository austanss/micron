#include "processes.h"

struct process_node {
    sys::proc::process* process;
    process_node*       next;
};


