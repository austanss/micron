#pragma once
#include "fs/vfs/vfs.h"

namespace fs {
    namespace ramfs {
        static fs::vfs::driver vfs_driver {
            .name           = "mnramfs",
            .read           = read,
            .write          = write,
            .create_file    = create_file,
            .delete_file    = delete_file
        };

        int read(fs::vfs::node* target, void* buffer);
        int write(fs::vfs::node* target, void* data);
        int create_file(fs::vfs::node* out, char* name);
        int delete_file(fs::vfs::node* target);
    }
}