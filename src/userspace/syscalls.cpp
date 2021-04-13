#include "types.h"
#include "devio/io.h"
#include "userio/kbd.h"
#include "memory/memory.h"
#include "userio/gfx.h"
#include "userio/tty.h"

extern "C" void sys_keyboard_event_subscribe(void (*handler)(io::keyboard::keyboard_packet))
{
    io::keyboard::keyboard_event_subscribe(handler);
}

extern "C" void sys_copy_framebuffer(uint32* buffer)
{
    memory::operations::memcpy (

        (void *)gfx::gop.framebuffer_addr, 

        buffer,

        gfx::gop.framebuffer_pitch 
        * gfx::gop.framebuffer_width 
        * gfx::gop.framebuffer_height 

    );
}

enum sys_info_field {
    total_ram,
    free_ram
};

extern "C" uint64 sys_get_info(uint08 field)
{
    switch (field)
    {
        case total_ram:
            return memory::total_memory_size;
        case free_ram:
            return memory::free_memory_size;
        default:
            return 0;
    }
}

extern "C" void sys_tty_print(char* text)
{
    io::tty::write(text);
}

extern "C" void* sys_allocate_page()
{
    void* page = memory::paging::allocation::request_page();
    memory::paging::donate_to_userspace(page);
    return page;
}