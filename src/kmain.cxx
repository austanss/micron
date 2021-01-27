#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/memory.hxx"
#include "kernel/logo.hxx"
#include "kernel/macros.hxx"
#include "kernel/kbd.hxx"
#include "kernel/font.hxx"
#include "kernel/io.hxx"
#include "kernel/bootinfo.hxx"
#include "kernel/uart.hxx"
#include "kernel/idt.hxx"
#include "kernel/gfx.hxx"
#include "kernel/power.hxx"
#include <cstdint>

#define __hang__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif


extern "C" {

// global constructors
void ctor_global
// required params
(/* none atm */)
// constructors called
{
	uart_initialize();
	serial_msg("GLOBAL CONSTRUCTORS CALLED\n");
}

void kernel_main(Boot_Info *bootloader_info)
{
	UEFI = bootloader_info->runtime_services;

	serial_msg("\nBOOTINFO ADDRESS: ");

	int2serial_hex((uintptr_t)bootloader_info);

	serial_msg("\nBOOTINFO CHECKSUM: ");

	int2serial_hex(bootloader_info->verification);

	serial_msg("\nFRAMEBUFFER ADDRESS: ");

	int2serial_hex(bootloader_info->vbe_framebuffer->framebuffer_base_addr);
	serial_msg("\n\n");

	map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);

	start_memory_manager();

	gop = *(bootloader_info->vbe_framebuffer);

	buffer = (uint32_t *)kmalloc(gop.x_resolution * gop.y_resolution * 4);

	Terminal& terminal = Terminal::instance();

	terminal.setCursor(0, 0);
	terminal << "\n  microNET  \n";
	terminal <<   "-----------+\n\n";

	terminal << "[$GREEN!kernel-startup$WHITE!] mapping memory" << status_eol;

	terminal << "[$GREEN!kernel-startup$WHITE!] starting malloc" << status_eol;

	terminal << "[$GREEN!kernel-startup$WHITE!] configuring terminal";
	terminal << status_eol;

	terminal << "[$GREEN!kernel-startup$WHITE!] starting keyboard" << status_eol;
	Keyboard::Initialize();

	serial_msg("\n\nKernel configured.");

	dimensions term_size = Terminal::get_optimal_size(dims(gop.x_resolution, gop.y_resolution));

	terminal << "[$GREEN!kernel-startup$WHITE!] res=" << gop.x_resolution << "x" << gop.y_resolution << " term=" << term_size.w << "x" << term_size.h << "\n";

	terminal << "[$GREEN!kernel-startup$WHITE!] boot success" << status_eol;
	terminal << "-=-" << status_eol;

}

}
