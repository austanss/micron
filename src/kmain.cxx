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
#include "kernel/tui.hxx"
#include "kernel/timer.hxx"

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif


extern "C" {

void kernel_main(stivale_struct *bootloader_info)
{	
	// do some startup configurationsur
	sys::config::calculate_kernel_size();
	sys::config::configure_memory(&(bootloader_info->framebuffer), &(bootloader_info->memory_map));
	sys::config::configure_graphics(&(bootloader_info->framebuffer));

	io::pit::set_frequency(1000);
	io::pit::pit_init();

	io::keyboard::init();

	// print out display information
	printf("\n\n\tdisplay: resolution is %dx%d\n", 
		gfx::gop.framebuffer_width, 
		gfx::gop.framebuffer_height);

	printf("\tframebuffer: at 0x%x for %d pages\n",
		gfx::gop.framebuffer_addr,
		(gfx::gop.framebuffer_height * gfx::gop.framebuffer_width * (gfx::gop.framebuffer_bpp / 8) / 4096 + 1));	
	
	gfx::shapes::dimensions term_size = terminal::get_optimal_size(gfx::shapes::dims(gfx::gop.framebuffer_width, gfx::gop.framebuffer_height));
	printf("\tterminal: sized %dx%d\n",
		term_size.w,
		term_size.h,
		terminal::text_buffer);

	// print out kernel position information
	printf("\tkernel: at 0x%x for %d pages\n",
		&sys::config::_kernel_start,
		sys::config::_kernel_pages);

	// print out memory information
	printf("\tmemory: %d MiB free\n",
		memory::free_memory_size / 0x400 / 0x400);	

	sys::tui::start();
}
}