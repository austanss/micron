#include "kernel/kconfigf.hxx"
#include "kernel/io.hxx"
#include "kernel/memory.hxx"
#include "kernel/terminal.hxx"
#include "kernel/gfx.hxx"
#include <cstdint>

uint64_t sys::config::_kernel_pages;
uint64_t sys::config::_kernel_size;

void sys::config::setup_paging(boot::boot_info* bootloader_info)
{
	memory::paging::pml_4 = (memory::paging::page_table *)memory::paging::allocation::request_page();
	
	memory::operations::memset(memory::paging::pml_4, 0, 0x1000);

	for (uint64_t t = (uint64_t)&_kernel_start; t < (uint64_t)&_kernel_start + _kernel_pages * 0x1000; t+=0x1000)
		memory::paging::map_memory((void *)t, (void *)t);

	uint64_t fb_base = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_base;
    uint64_t fb_size = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_size + 0x1000;

    for (uint64_t t = fb_base; t < fb_base + fb_size; t += 4096)
        memory::paging::map_memory((void*)t, (void*)t);

io::serial::serial_msg("pml4 address: ");
io::serial::serial_msg(util::itoa((uint64_t)memory::paging::pml_4, 16));
io::serial::serial_msg("\n");

	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	
}

void sys::config::configure_memory(boot::boot_info* bootloader_info)
{
    memory::allocation::map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);
    sys::config::setup_paging(bootloader_info);
    memory::allocation::start_malloc();
}

void sys::config::configure_graphics(boot::boot_info* bootloader_info)
{
    gfx::gop = *(bootloader_info->vbe_framebuffer);

	gfx::buffer = (uint32_t *)memory::allocation::kmalloc(gfx::gop.framebuffer_size);

	terminal::instance();
}

void sys::config::calculate_kernel_size()
{
    _kernel_size = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    _kernel_pages = (uint64_t)_kernel_size / 4096 + 1;
}