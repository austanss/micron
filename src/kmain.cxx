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

	gop = *(bootloader_info->vbe_framebuffer);

	Terminal& terminal = Terminal::instance();

	terminal.setCursor(0, 0);
	terminal << logo;
	terminal << status_eol;

	terminal.staticLogo = true;

	terminal << status_pend << "Initializing keyboard..." << status_eol;
	Keyboard::Initialize();
	terminal << status_good << "Keyboard operational!" << status_eol;

	terminal << status_pend << "Setting up paging..." << status_eol;
//	begin_paging();
	terminal << status_fail << "Paging has been manually disabled." << status_eol;

	terminal << status_pend << "Mapping memory..." << status_eol;
	memory_info* mem_info = map_memory(bootloader_info->memory_map, bootloader_info->mmap_size, bootloader_info->mmap_descriptor_size);
	terminal << status_good << "Memory has been mapped!" << status_eol;

	terminal << status_pend << "Starting memory manager..." << status_eol;
	start_memory_manager(mem_info);
	terminal << status_good << "Managing memory!" << status_eol;

	serial_msg("\n\nKernel configured.");

	dimensions term_size = Terminal::get_optimal_size(dims(gop.x_resolution, gop.y_resolution));

	terminal << status_pend << "Current display resolution: " << gop.x_resolution << "x" << gop.y_resolution << status_eol;

	terminal << status_pend << "Current terminal size: " << term_size.w << "x" << term_size.h << status_eol;

	terminal << status_good << "microNET: boot success" << status_eol;
	terminal << terminal_line << status_eol;

	terminal << "Testing itoa: " << itoa(7, 10) << status_eol;
}

}
