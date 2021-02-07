#include "kernel/kconfigf.hxx"
#include "kernel/boot.hxx"
#include "kernel/io.hxx"
#include "kernel/kutil.hxx"
#include "kernel/memory.hxx"
#include "kernel/terminal.hxx"
#include "kernel/gfx.hxx"
#include "kernel/bitmap.hxx"
#include <stdint.h>

uint64_t sys::config::_kernel_pages;
uint64_t sys::config::_kernel_size;

extern util::bitmap page_bitmap_map;

void sys::config::setup_paging(boot::boot_info* bootloader_info)
{
	memory::paging::pml_4 = (memory::paging::page_table *)memory::paging::allocation::request_page();
	
	memory::operations::memset(memory::paging::pml_4, 0, 0x1000);

    memory::paging::map_memory((void*)memory::paging::pml_4, (void*)memory::paging::pml_4);
    
	for (uint64_t t = 0x0; t < memory::allocation::get_total_memory_size(nullptr, NULL, NULL); t+=0x1000)
	    { memory::paging::map_memory((void *)t, (void *)t); }

	uint64_t fb_base = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_base;
    uint64_t fb_size = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_size + 0x1000;

    for (uint64_t t = fb_base; t < fb_base + fb_size; t += 4096)
        { memory::paging::map_memory((void*)t, (void*)t); }


	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	
}

void sys::config::configure_memory(boot::boot_info* bootloader_info)
{
    memory::allocation::map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);
    sys::config::setup_paging(bootloader_info);
    memory::allocation::start_allocator();
}

void sys::config::configure_graphics(boot::boot_info* bootloader_info)
{
    gfx::gop = *(bootloader_info->vbe_framebuffer);

	gfx::buffer = (uint32_t *)memory::paging::allocation::request_pages(gfx::gop.framebuffer_size / 0x1000 + 1);

	terminal::instance();
}

void sys::config::calculate_kernel_size()
{
    _kernel_size = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    _kernel_pages = (uint64_t)_kernel_size / 4096 + 1;
}

void sys::config::boot_info_copy(boot::boot_info* dst, boot::boot_info* src)
{
    dst->verification = src->verification;
    dst->mmap_size = src->mmap_size;
    dst->mmap_descriptor_size = src->mmap_descriptor_size;
    dst->memory_map = src->memory_map;
    dst->runtime_services = src->runtime_services;
    dst->vbe_framebuffer->y_resolution = src->vbe_framebuffer->y_resolution;
    dst->vbe_framebuffer->x_resolution = src->vbe_framebuffer->x_resolution;
    dst->vbe_framebuffer->framebuffer_base = src->vbe_framebuffer->framebuffer_base;
    dst->vbe_framebuffer->framebuffer_mode = src->vbe_framebuffer->framebuffer_mode;
    dst->vbe_framebuffer->framebuffer_size = src->vbe_framebuffer->framebuffer_size;
    dst->vbe_framebuffer->pixels_per_scan_line = src->vbe_framebuffer->pixels_per_scan_line;
}