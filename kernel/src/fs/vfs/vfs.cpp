#include "vfs.h"
#include "memory/heap.h"
#include "util/kutil.h"

fs::vfs::node et_al {
    "&:",
    0xFF,
    nullptr,
    nullptr
};

fs::vfs::node* fs::vfs::add_default_node(char* name)
{
    node* walker;
    for (walker = &et_al; !!walker->next; walker = walker->next);

    walker->next = (node *)memory::heap::malloc(sizeof(node));

    walker = walker->next;

    util::strcpy((char *)&walker->identifier, name);
    walker->permissions = (writable | cacheable | visible | movable);
    walker->filesystem = default_driver;
    walker->next = nullptr;
}

fs::vfs::node* fs::vfs::get_node(char* name)
{
    node* walker;
    for (walker = &et_al; !!walker; walker = walker->next)
        if (util::strcomp(name, walker->identifier))
            break;

    return walker;
}

void fs::vfs::delete_node(char* name)
{
    node* walker;
    for (walker = &et_al; !!walker->next; walker = walker->next)
        if (util::strcomp(name, walker->next->identifier))
            break;

    if (!walker->next)
        return;

    node* ahead = walker->next->next;

    walker->next->filesystem->delete_file(walker->next);

    memory::heap::free(walker->next);

    walker->next = ahead;
}