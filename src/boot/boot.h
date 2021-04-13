#pragma once

#include "types.h"
#include "devio/acpi.h"

/* --- Header --------------------------------------------------------------- */
/*  Information passed from the kernel to the bootloader                      */



struct stivale_header {
    address stack;
    word flags;
    uint16 framebuffer_width;
    uint16 framebuffer_height;
    uint16 framebuffer_bpp;
    address entry_point;
} __attribute__((packed));

/* --- Struct --------------------------------------------------------------- */
/*  Information passed from the bootloader to the kernel                      */

struct stivale_module {
    address begin;
    address end;
    char string[128];
    address next;
} __attribute__((__packed__));

#define STIVALE_MMAP_USABLE                 1
#define STIVALE_MMAP_RESERVED               2
#define STIVALE_MMAP_ACPI_RECLAIMABLE       3
#define STIVALE_MMAP_ACPI_NVS               4
#define STIVALE_MMAP_BAD_MEMORY             5
#define STIVALE_MMAP_KERNEL_AND_MODULES     10
#define STIVALE_MMAP_BOOTLOADER_RECLAIMABLE 0x1000

struct stivale_mmap_entry {
    address base;
    uint64 length;
    uint32 type;
    uint32 unused;
} __attribute__((packed));

struct stivale_framebuffer {
    address framebuffer_addr;
    uint16  framebuffer_pitch;
    uint16  framebuffer_width;
    uint16  framebuffer_height;
    uint16  framebuffer_bpp;
} __attribute__((packed));

struct stivale_memory_map {
    address memory_map_addr;
    uint64  memory_map_entries;
} __attribute__((packed));

#define STIVALE_FBUF_MMODEL_RGB 1

struct stivale_struct {
    address cmdline;
    stivale_memory_map  memory_map;
    stivale_framebuffer framebuffer;
    sys::acpi::rsdp2*     rsdp;
    uint64  module_count;
    address modules;
    uint64  epoch;
    qword   flags; // bit 0: 1 if booted with BIOS, 0 if booted with UEFI
                    // bit 1: 1 if extended colour information passed, 0 if not
    uint08  fb_memory_model;
    uint08  fb_red_mask_size;
    uint08  fb_red_mask_shift;
    uint08  fb_green_mask_size;
    uint08  fb_green_mask_shift;
    uint08  fb_blue_mask_size;
    uint08  fb_blue_mask_shift;
} __attribute__((packed));