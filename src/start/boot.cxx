//
// Created by rizet on 11/27/20.
//

#include "start/boot.hxx"

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivalehdr"), used))
struct stivale_header stivale_hdr = {

    .stack = 0,

    .flags = 0b001,
    
    .framebuffer_width = 0,    

    .framebuffer_height = 0,
    
    .framebuffer_bpp = 32,
    
    .entry_point = 0,
};