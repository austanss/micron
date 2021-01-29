#include <cstdint>
#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/kbd.hxx"
#include "kernel/io.hxx"
#include "kernel/boot.hxx"
#include "kernel/idt.hxx"
#include "kernel/gfx.hxx"
#include "kernel/power.hxx"

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

extern uint64_t _kernel_start;
extern uint64_t _kernel_end;

extern "C" {

// global constructors
void ctor_global
// required params
(/* none atm */)
// constructors called
{
	io::serial::serial_msg("GLOBAL CONSTRUCTORS CALLED\n");
}

void kernel_main(boot::boot_info *bootloader_info)
{
	uint64_t _kernel_size = (uint64_t)&_kernel_end - (uint64_t)&_kernel_start;
    uint64_t _kernel_pages = (uint64_t)_kernel_size / 4096 + 1;

	boot::uefi = bootloader_info->runtime_services;

	io::serial::serial_msg("\nBOOTINFO ADDRESS: ");

	io::serial::serial_msg(util::itoa((uintptr_t)bootloader_info, 16));

	io::serial::serial_msg("\nBOOTINFO CHECKSUM: ");

	io::serial::serial_msg(util::itoa(bootloader_info->verification, 16));

	io::serial::serial_msg("\nFRAMEBUFFER ADDRESS: ");

	io::serial::serial_msg(util::itoa((uint64_t)bootloader_info->vbe_framebuffer->framebuffer_base, 16));
	io::serial::serial_msg("\n\n");

	memory::allocation::map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);

	memory::paging::pml_4 = (memory::paging::page_table *)memory::paging::allocation::request_page();
	
	memory::operations::memset(memory::paging::pml_4, 0, 0x1000);

	for (uint64_t t = 0; t < memory::allocation::get_total_memory_size(bootloader_info->memory_map, 
																		bootloader_info->mmap_size / bootloader_info->mmap_descriptor_size, 
																		bootloader_info->mmap_descriptor_size); t+= 0x1000) 
    																	    memory::paging::map_memory((void*)t, (void*)t);
    
	uint64_t fb_base = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_base;
    uint64_t fb_size = (uint64_t)bootloader_info->vbe_framebuffer->framebuffer_size + 0x1000;

    for (uint64_t t = fb_base; t < fb_base + fb_size; t += 4096)
        memory::paging::map_memory((void*)t, (void*)t);

	asm volatile ("mov %0, %%cr3" : : "r" (memory::paging::pml_4));	

//	memory::allocation::start_malloc();

	gfx::gop = *(bootloader_info->vbe_framebuffer);

	gfx::buffer = (uint32_t *)memory::allocation::kmalloc(gfx::gop.framebuffer_size + 3) + 3;

	*((uint8_t *)gfx::buffer - 3) = 'D';
	*((uint8_t *)gfx::buffer - 2) = 'F';
	*((uint8_t *)gfx::buffer - 1) = 'B';

	terminal::instance();
	
	printf("%s", "Booting...\n");

	printf("[kernel-startup] configuring paging\n");

	printf("[kernel-startup] mapping memory\n");

	printf("[kernel-startup] starting malloc\n");

	printf("[kernel-startup] configuring terminal\n");

	printf("[kernel-startup] starting keyboard\n");
	io::keyboard::init();

	io::serial::serial_msg("\n\nKernel configured.");

	gfx::shapes::dimensions term_size = terminal::get_optimal_size(gfx::shapes::dims(gfx::gop.x_resolution, gfx::gop.y_resolution));

	printf("[kernel-startup] res=%dx%d term=%dx%d\n", 
		gfx::gop.x_resolution, 
		gfx::gop.y_resolution, 
		term_size.w, 
		term_size.h);

	printf("[kernel-startup] kernel loaded at: 0x%x for %d pages\n",
		&_kernel_start,
		_kernel_pages);

	printf("[kernel-startup] boot success\n");

	int* tf25 = (int *)memory::allocation::kmalloc(4);

	memory::operations::memset(tf25, 25, 1);

	printf("[kernel-checkup] testing memset: \nin: %d | out: %d", 25, *tf25);
}

}
