#pragma once
#include "syscalls.h"

struct system_info {

    struct {
        unsigned long fb_ad;
        unsigned short fb_resx;
        unsigned short fb_resy;
        unsigned short fb_pitch;
        unsigned short fb_bpp;
        unsigned long fb_size;
    } display_info;

    struct {
        unsigned long total_ram;
        unsigned long used_ram;
        unsigned long free_ram;
    } memory_info;
};

inline system_info* get_info()
{
    system_info* rax;
    asm volatile ("syscall" : "=a" (rax) : "a" (sysinfo));
    return rax;
}