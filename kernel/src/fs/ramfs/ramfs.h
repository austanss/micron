#pragma once
#include "fs/vfs/vfs.h"

namespace fs {
    namespace ramfs {

        int read(fs::vfs::node* target, void* buffer, uint position, uint bytes);
        int write(fs::vfs::node* target, void* data, uint position, uint bytes);
        int create_file(fs::vfs::node* target);
        int delete_file(fs::vfs::node* target);

        static fs::vfs::driver vfs_driver {
            .name           = (char *)"mnramfs",
            .read           = read,
            .write          = write,
            .create_file    = create_file,
            .delete_file    = delete_file
        };
    }
}