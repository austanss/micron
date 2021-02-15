#include <stdint.h>
#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/kbd.hxx"
#include "kernel/io.hxx"
#include "kernel/boot.hxx"
#include "kernel/idt.hxx"
#include "kernel/gfx.hxx"
#include "kernel/power.hxx"
#include "kernel/kconfigf.hxx"
#include "kernel/serialcon.hxx"

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

extern "C" {

void kernel_main(boot::boot_info *bootloader_info)
{	
	// do some startup configurations
	sys::config::calculate_kernel_size();
	boot::uefi = bootloader_info->runtime_services;
	sys::config::configure_memory(bootloader_info);
	sys::config::configure_graphics(bootloader_info);

	io::keyboard::init();

	// print out display information
	printf("display: resolution is %dx%d\n", 
		gfx::gop.x_resolution, 
		gfx::gop.y_resolution);

	printf("framebuffer: at 0x%x for %d pages\n",
		gfx::gop.framebuffer_base,
		gfx::gop.framebuffer_size / 4096 + 1);	

	// print out kernel position information
	printf("kernel: at 0x%x for %d pages\n",
		&sys::config::_kernel_start,
		sys::config::_kernel_pages);

	printf("memory: %d MiB free\n",
		memory::free_memory_size / 0x400 / 0x400);	

	gfx::shapes::dimensions term_size = terminal::get_optimal_size(gfx::shapes::dims(gfx::gop.x_resolution, gfx::gop.y_resolution));
	printf("terminal: sized %dx%d\n",
		term_size.w,
		term_size.h);
}
}