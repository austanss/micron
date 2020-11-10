#include "kernel/kutil.hxx"
#include "kernel/terminal.hxx"
#include "kernel/mem.hxx"
#include "kernel/logo.hxx"
#include "kernel/macros.hxx"
#include "kernel/kbd.hxx"
#include "kernel/font.hxx"
#include "kernel/io.hxx"
#include "kernel/bootinfo.hxx"
#include "kernel/uart.hxx"
#include "kernel/idt.hxx"
#include "kernel/gfx.hxx"
#include <common/bootinfo_addr.h>

#define __stop__ while (true);

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

#define OS_CONTINOUS_RAM_START 0x100000

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

void kernel_main()
{
//	bootloader_info = (Boot_Info *)0xBEEF;
	Terminal& terminal = Terminal::instance();

	Boot_Info *bootloader_info = (Boot_Info *)BOOTINFO_ADDRESS;

	register uint64_t *foo asm("rax");

	terminal.setCursor(0, 0);
	terminal << logo;
	terminal << status_eol;

	terminal.staticLogo = true;

	serial_msg("\nBOOTINFO ADDRESS: ");

	hex_str_serial((uintptr_t)bootloader_info);

	serial_msg("\nBOOTINFO CHECKSUM: ");

	hex_str_serial(bootloader_info->verification);

	serial_msg("\nFRAMEBUFFER ADDRESS: ");

	hex_str_serial(bootloader_info->vbe_framebuffer.framebuffer_base_addr);

	plot_pixel(200, 300, 0xFFFF00);
	plot_pixel(201, 300, 0xFFFF00);
	plot_pixel(200, 301, 0xFFFF00);
	plot_pixel(201, 301, 0xFFFF00);

	terminal << status_pend << "Initializing keyboard..." << status_eol;
	Keyboard::Initialize();
	terminal << status_good << "Keyboard operational!" << status_eol;


	terminal << status_pend << "Setting up paging..." << status_eol;
//	beginPaging();
	terminal << status_fail << "Paging has been manually disabled." << status_eol;

	terminal << status_good << "Console font is located at: ";
	writeHex(_mainfont);
	terminal << status_eol;

	terminal << status_good << "microNET: boot success (microCORE architecture " << ARCH << ")" << status_eol;

	while (true);
//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();
}

}
