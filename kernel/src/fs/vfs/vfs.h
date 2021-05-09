#pragma once
#include "types.h"

namespace fs {
    namespace vfs {
        constexpr uint16 MAX_FILENAME_LENGTH    = 256;

        enum permission_fields : uint08 {
            writable    = 1 << 0,
            visible     = 1 << 1,
            runlocked   = 1 << 2,
            wunlocked   = 1 << 3,
            multistream = 1 << 4,
            movable     = 1 << 5,
            cacheable   = 1 << 6,
            executable  = 1 << 7
        };

        struct node;

        struct driver {
            char* name;
            int (*read )        (node* target,  void* buffer);
            int (*write)        (node* target,  void* data);
            int (*create_file)  (node* out,     char* name);
            int (*delete_file)  (node* target);
        };

        struct node {
            char        identifier[MAX_FILENAME_LENGTH];
            uint08      permissions;
            driver*     filesystem;
            node*       parent;
            node*       next;
            node*       child;
        };

        node*   add_default_node(node* parent, char* name);
        node*   get_node(char* name);
    }
}