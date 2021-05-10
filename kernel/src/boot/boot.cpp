//
// Created by rizet on 11/27/20.
//

#include "boot/boot.h"

stivale2_header_tag_framebuffer stivale2_framebuffer_tag = {
    
    .tag = {
        
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next       = nullptr
    },

    .framebuffer_width = 0, // set it yourself, limine
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

// The stivale2 specification says we need to define a "header structure".
// This structure needs to reside in the .stivale2hdr ELF section in order
// for the bootloader to find it. We use this __attribute__ directive to
// tell the compiler to put the following structure in said section.
__attribute__((section(".stivale2hdr"), used))
stivale2_header stivale2_hdr = {

    .entry_point    = (uint64)nullptr,
    .stack          = (uint64)nullptr,
    .flags          = 0x00,
    .tags           = (uint64)&stivale2_framebuffer_tag
};

stivale2_tag* find_tag(stivale2_tag* first, uint64 tag_id)
{
    for (first = first; !!first; first = first->next)
    {
        if (first->identifier == tag_id)
            return first;
    }
    return nullptr;
}