#include "vfs.h"
#include "memory/heap.h"
#include "util/kutil.h"
#include "drivers/uart/serial.h"

fs::vfs::node vfs_root {
    "vfr",
    0xFF,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};

fs::vfs::node* fs::vfs::add_default_node(fs::vfs::node* parent, char* name)
{
    fs::vfs::node* new_node_location = parent->child;

    if (!new_node_location)
        parent->child = (fs::vfs::node *)memory::heap::malloc(sizeof(fs::vfs::node));
    else
        while (!!new_node_location->next->next)
            new_node_location = new_node_location->next;

    new_node_location->next = (fs::vfs::node *)memory::heap::malloc(sizeof(fs::vfs::node));
    new_node_location = new_node_location->next;
    
    util::strcpy((char *)&new_node_location->identifier, name);

    new_node_location->permissions  = (writable | visible | movable | multistream | cacheable);
    new_node_location->child        = nullptr;
    new_node_location->parent       = parent;
    new_node_location->next         = nullptr;
    new_node_location->filesystem   = nullptr;

    return new_node_location;
}

fs::vfs::node* fs::vfs::get_node(char* name)
{
    char** strspl = util::strsplit(name, ':');
    char* volume_label = (char *)memory::heap::malloc(util::strlen(strspl[0] + 1));
    util::strcpy(volume_label, strspl[0]);
    memory::heap::free(strspl);
    memory::heap::free(strspl[0]);
    name += util::strlen(volume_label) + 1;
    char** subnode_names = util::strsplit(name, '/');

    node* walking_node = &vfs_root;

    for (walking_node = walking_node->child; !!walking_node->next; walking_node = walking_node->next)
        if (util::strcomp(walking_node->identifier, volume_label))
            break;

    
    (void)subnode_names;
    

    return nullptr;
}