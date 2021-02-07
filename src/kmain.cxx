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

boot::boot_info bootloader_info;
boot::gop_framebuffer vbe_framebuffer;

extern "C" {

void kernel_main(boot::boot_info *boot_info)
{
	// globalize boot_info because soon it will become invalid
	// due to paging and raise page faults, which will crash
	bootloader_info.vbe_framebuffer = &vbe_framebuffer;
	sys::config::boot_info_copy(&bootloader_info, boot_info);

	// do some startup configurations
	sys::config::calculate_kernel_size();
	boot::uefi = boot_info->runtime_services;
	sys::config::configure_memory(&bootloader_info);
	sys::config::configure_graphics(&bootloader_info);
	io::serial::console::init();	

	
	// now terminal is started, print out statuses for things we have already done
	printf("%s", "Booting...\n");
	printf("[kernel-startup] configuring paging\n");
	printf("[kernel-startup] mapping memory\n");
	printf("[kernel-startup] starting malloc\n");
	printf("[kernel-startup] configuring terminal\n");

	// start keyboard
	printf("[kernel-startup] starting keyboard\n");
	io::keyboard::init();

	// print out display information
	gfx::shapes::dimensions term_size = terminal::get_optimal_size(gfx::shapes::dims(gfx::gop.x_resolution, gfx::gop.y_resolution));
	printf("[kernel-startup] res=%dx%d term=%dx%d\n", 
		gfx::gop.x_resolution, 
		gfx::gop.y_resolution, 
		term_size.w, 
		term_size.h);

	// print out kernel position information
	printf("[kernel-startup] kernel loaded at: 0x%x for %d pages\n",
		&sys::config::_kernel_start,
		sys::config::_kernel_pages);

	// give the all-clear
	printf("[kernel-startup] boot success\n");
}
}