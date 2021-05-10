#include "types.h"
#include "drivers/io.h"
#include "drivers/kbd/kbd.h"
#include "memory/pmm.h"
#include "memory/paging.h"
#include "memory/operations.h"
#include "drivers/gfx/gop/gop.h"
#include "drivers/tty/tty.h"

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

extern "C" void sys_pmap()
{

}

extern "C" void sys_pexe()
{

}

extern "C" void sys_punmap()
{

}

struct system_info {

    struct {
        uint64 fb_ad;
        uint32 fb_resx;
        uint32 fb_resy;
        uint64 fb_pitch;
        uint64 fb_bpp;
    } display_info;

    struct {
        uint64 total_ram;
        uint64 used_ram;
        uint64 free_ram;
    } memory_info;
};

system_info special_info {

};

extern "C" system_info* sys_sinfo()
{
    return &special_info;
}