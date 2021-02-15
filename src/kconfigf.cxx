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
    
    for (int t = 0; t < 0x100; t++) { 
        void* pos = (void*)0xffff800000000000;
        memory::paging::map_memory(pos, memory::paging::allocation::request_page());
        pos = (void*)((uint64_t)pos + 0x1000);
    }

    memory::allocation::initialize_heap((void*)0xffff800000000000, 0x100000);
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