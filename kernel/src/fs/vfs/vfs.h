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
            int (*read )        (node* target, void* buffer, uint position, uint bytes);
            int (*write)        (node* target, void* data, uint position, uint bytes);
            int (*create_file)  (node* target);
            int (*delete_file)  (node* target);
        };

        struct node {
            char        identifier[MAX_FILENAME_LENGTH];
            uint08      permissions;
            driver*     filesystem;
            node*       next;
        };

        extern driver* default_driver;

        node*   add_default_node(char* name);
        void    delete_node(char* name);
        node*   get_node(char* name);
    }
}