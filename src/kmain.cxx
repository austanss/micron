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
#include <common/bootinfo_addr.hxx>

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

	Boot_Info *bootloader_info = (Boot_Info *)BOOTINFO_ADDRESS;

	register uint64_t *foo asm("rax");


	serial_msg("\nBOOTINFO ADDRESS: ");

	hex_str_serial((uintptr_t)bootloader_info);

	serial_msg("\nBOOTINFO CHECKSUM: ");

	hex_str_serial(bootloader_info->verification);

	serial_msg("\nFRAMEBUFFER ADDRESS: ");

	hex_str_serial(bootloader_info->vbe_framebuffer.framebuffer_base_addr);

	gop = bootloader_info->vbe_framebuffer;

	Terminal& terminal = Terminal::instance();

//	terminal.setCursor(0, 0);
//	terminal << logo;
//	terminal << status_eol;

//	terminal.staticLogo = true;

	terminal.put_entry_at('C', 0, 0, 0);

//	terminal << status_pend << "Initializing keyboard..." << status_eol;
	Keyboard::Initialize();
//	terminal << status_good << "Keyboard operational!" << status_eol;

	plot_pixel(300, 200, 0xFFFF00);
	plot_pixel(300, 201, 0xFFFF00);
	plot_pixel(300, 202, 0xFFFF00);
	plot_pixel(301, 200, 0xFFFF00);
	plot_pixel(301, 201, 0xFFFF00);
	plot_pixel(301, 202, 0xFFFF00);
	plot_pixel(302, 200, 0xFFFF00);
	plot_pixel(302, 201, 0xFFFF00);
	plot_pixel(302, 202, 0xFFFF00);

//	terminal << status_pend << "Setting up paging..." << status_eol;
//	beginPaging();
//	terminal << status_fail << "Paging has been manually disabled." << status_eol;

//	terminal << status_good << "Console font is located at: ";
//	writeHex(_mainfont);
//	terminal << status_eol;

//	terminal << status_good << "microNET: boot success (microCORE architecture " << ARCH << ")" << status_eol;

	rect(200, 300, 20, 20, 0xFFFF00);
//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();
}

}
