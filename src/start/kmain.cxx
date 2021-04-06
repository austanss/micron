#include "types.hxx"
#include "util/kutil.hxx"
#include "output/terminal.hxx"
#include "memory/memory.hxx"
#include "input/kbd.hxx"
#include "io/io.hxx"
#include "start/boot.hxx"
#include "cpu/idt.hxx"
#include "output/gfx.hxx"
#include "io/power.hxx"
#include "config/kconfigf.hxx"
#include "input/serialcon.hxx"
#include "input/tui.hxx"
#include "chrono/timer.hxx"
#include "output/speaker.hxx"
#include "cpu/tss.hxx"
#include "io/disk/ahci.hxx"

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif


extern "C" {

void kernel_main(stivale_struct *bootloader_info, uint stack)
{	
	// do some startup configurations
	sys::config::calculate_kernel_size();
	sys::config::configure_memory(&(bootloader_info->framebuffer), &(bootloader_info->memory_map));
	cpu::tss::tss_install(0, stack);
	cpu::idt::load_idt();
	asm volatile ("sti");
	sys::config::configure_graphics(&(bootloader_info->framebuffer));
	// print out pci stuffs
	sys::config::configure_pci(bootloader_info->rsdp);

	io::pit::set_c0_frequency(1000);
	io::pit::pit_init();

	terminal::instance().setCursor(11, terminal::instance().VGA_HEIGHT - 6);
	terminal::instance().write("$LIGHT_GREY! microNET");
	terminal::instance().setCursor(terminal::instance().VGA_WIDTH - 24, terminal::instance().VGA_HEIGHT - 6);
	terminal::instance().write("$LIGHT_GREY!v0.6 \"Glassi\"");
	uint color = 0x88AACC;
	for (uint i = 55; i < gfx::gop.framebuffer_width - (uint)55; i++)
	{
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 28), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 29), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 30), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 31), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 32), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 33), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 34), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 35), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 36), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 37), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 38), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 39), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 40), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 41), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 42), color);
		gfx::screen::plot_pixel(gfx::shapes::pos(i, gfx::gop.framebuffer_height - 43), color);
		sys::chrono::sleep(1);
	}

	sys::audio::pcspk::init();
	sys::chrono::sleep(100);
	sys::audio::pcspk::beep(900, 100);
	sys::chrono::sleep(20);
	sys::audio::pcspk::beep(100, 100);
	terminal::instance().setCursor(0, terminal::instance().VGA_HEIGHT - 9);
	terminal::instance().write(terminal_line);
	terminal::instance().setCursor(0, 0);


	sys::audio::pcspk::init();
	sys::audio::pcspk::beep();

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
		&sys::config::__kernel_start,
		sys::config::__kernel_pages);

	// print out memory information
	printf("\tmemory: %d MiB free\n",
		memory::free_memory_size / 0x400 / 0x400);	

//	sys::tui::start();
 
	sys::config::configure_userspace();
}
}