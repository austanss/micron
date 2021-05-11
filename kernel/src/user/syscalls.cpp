#include "types.h"
#include "drivers/io.h"
#include "drivers/kbd/kbd.h"
#include "memory/pmm.h"
#include "memory/paging.h"
#include "memory/operations.h"
#include "drivers/gfx/gop/gop.h"
#include "drivers/tty/tty.h"
#include "drivers/evsys/evsys.h"

extern "C" void sys_open(const char* filename, uint08 flags)
{

}

extern "C" void sys_read()
{

}

extern "C" void sys_write()
{

}

extern "C" void sys_close()
{

}

extern "C" void* sys_pmap(void* vaddress)
{
    memory::paging::map_memory(vaddress, memory::pmm::request_page(), false);
    memory::paging::donate_to_userspace(vaddress);
    return vaddress;
}

extern "C" void sys_pexe(void* vaddress)
{
    memory::pmm::free_page(memory::paging::get_physical_address(vaddress));
}

extern "C" void sys_punmap()
{

}

struct system_info {

    struct {
        uint64 fb_ad;
        uint16 fb_resx;
        uint16 fb_resy;
        uint16 fb_pitch;
        uint16 fb_bpp;
    } display_info;

    struct {
        uint64 total_ram;
        uint64 used_ram;
        uint64 free_ram;
    } memory_info;
};

system_info special_info;

extern "C" system_info* sys_sinfo()
{
    special_info = {
        .display_info {
            .fb_ad      = gfx::gop.framebuffer_addr,
            .fb_resx    = gfx::gop.framebuffer_width,
            .fb_resy    = gfx::gop.framebuffer_height,
            .fb_pitch   = gfx::gop.framebuffer_pitch,
            .fb_bpp     = gfx::gop.framebuffer_bpp
        },

        .memory_info {
            .total_ram  = memory::pmm::total_memory_size,
            .used_ram   = memory::pmm::used_memory_size,
            .free_ram   = memory::pmm::total_memory_size - memory::pmm::used_memory_size
        }
    };
    return &special_info;
}

extern "C" void sys_levrd(sys::evsys::evsys_recipient_descriptor* descriptor)
{
    sys::evsys::register_recipient(descriptor);
}

extern "C" void sys_devrd(sys::evsys::evsys_recipient_descriptor* descriptor)
{
    sys::evsys::deregister_recipient(descriptor);
}