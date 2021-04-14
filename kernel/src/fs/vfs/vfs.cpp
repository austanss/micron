#include "vfs.h"
#include "memory/heap.h"
#include "util/kutil.h"
#include "drivers/uart/serial.h"

fs::vfs::node* fs::vfs::add_default_node(fs::vfs::node* parent, char* name)
{
    parent->child = (fs::vfs::node *)memory::heap::malloc(sizeof(fs::vfs::node));
    util::strcpy((char *)&parent->child->identifier, name);
    return parent->child;
}

fs::vfs::node* fs::vfs::get_node(char* name)
{
    char** strspl = util::strsplit(name, ':');
    char* volume_label = (char *)memory::heap::malloc(util::strlen(strspl[0] + 1));
    util::strcpy(volume_label, strspl[0]);
    memory::heap::free(strspl);
    memory::heap::free(strspl[0]);
    name += util::strlen(volume_label);
    char** subnode_names = util::strsplit(name, '/');
    (void) subnode_names;
    io::serial::serial_msg("attempt to open: ");
    io::serial::serial_msg(name);
    io::serial::serial_msg(" from volume: ");
    io::serial::serial_msg(volume_label);
    io::serial::serial_msg("\n");
    return nullptr;
}