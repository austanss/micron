#include "ramfs.h"
#include "memory/pmm.h"
#include "util/kutil.h"
#include "memory/heap.h"
#include "memory/operations.h"

struct ramfs_file {
    char        identifier[fs::vfs::MAX_FILENAME_LENGTH];
    void*       data;
    uint        pages;
    ramfs_file* next;
};

ramfs_file ramfs_rootdir {
    "",
    nullptr,
    0,
    nullptr
};

int fs::ramfs::read(fs::vfs::node* target, void* buffer, uint position, uint bytes)
{
    ramfs_file* walker;

    for (walker = &ramfs_rootdir; !!walker; walker = walker->next)
        if (util::strcomp(walker->identifier, target->identifier))
            break;

    if (!walker)
        return -1;

    if (position + bytes > walker->pages * 0x1000)
        return -2;

    memory::operations::memcpy(buffer, (void *)((address)walker->data + position), bytes);

    return 0;
}


int fs::ramfs::write(fs::vfs::node* target, void* data, uint position, uint bytes)
{
    ramfs_file* walker;

    for (walker = &ramfs_rootdir; !!walker; walker = walker->next)
        if (util::strcomp(walker->identifier, target->identifier))
            break;

    if (!walker)
        return -1;

    if (position + bytes > walker->pages * 0x1000)
    {
        uint extra_bytes = position + bytes - walker->pages * 0x1000;
        uint extra_pages = extra_bytes / 0x1000 + 1;
        
        walker->data = memory::pmm::reallocate_pool(walker->data, walker->pages + extra_pages);

        walker->pages += extra_pages;
    }

    memory::operations::memcpy((void *)((address)walker->data + position), data, bytes);

    return 0;
}


int fs::ramfs::create_file(fs::vfs::node* target)
{
    ramfs_file* walker;

    for (walker = &ramfs_rootdir; !!walker->next; walker = walker->next);

    if (!walker)
        return -1;

    walker->next = (ramfs_file *)memory::heap::malloc(sizeof(ramfs_file));

    walker = walker->next;

    walker->next = nullptr;

    util::strcpy(walker->identifier, target->identifier);
    walker->data = memory::pmm::request_pool(2);
    walker->pages = 1;

    target->filesystem = &vfs_driver;

    return 0;
}

int fs::ramfs::delete_file(fs::vfs::node* target)
{
    ramfs_file* walker;

    for (walker = &ramfs_rootdir; !!walker->next; walker = walker->next)
        if (util::strcomp(walker->next->identifier, target->identifier))
            break;

    if (!walker->next)
        return 0;

    memory::pmm::free_pool(walker->next->data);

    ramfs_file* ahead = walker->next->next;

    memory::heap::free(walker->next);

    walker->next = ahead;

    return 0;
}