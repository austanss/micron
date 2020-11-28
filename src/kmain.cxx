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
#include "kernel/power.hxx"

#define __hang__ while (true);

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

void kernel_main(Boot_Info *bootloader_info)
{
	UEFI = bootloader_info->runtime_services;

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

	terminal.put_entry_at('H', 0x00FFFF, 0,	 0);
	terminal.put_entry_at('e', 0x00FFFF, 1,	 0);
	terminal.put_entry_at('l', 0x00FFFF, 2,	 0);
	terminal.put_entry_at('l', 0x00FFFF, 3,	 0);
	terminal.put_entry_at('o', 0x00FFFF, 4,	 0);
	terminal.put_entry_at(',', 0x00FFFF, 5,	 0);
	terminal.put_entry_at(' ', 0x00FFFF, 6,	 0);
	terminal.put_entry_at('w', 0x00FFFF, 7,	 0);
	terminal.put_entry_at('o', 0x00FFFF, 8,	 0);
	terminal.put_entry_at('r', 0x00FFFF, 9,	 0);
	terminal.put_entry_at('l', 0x00FFFF, 10, 0);
	terminal.put_entry_at('d', 0x00FFFF, 11, 0);
	terminal.put_entry_at('!', 0x00FFFF, 12, 0);

//	terminal << status_pend << "Initializing keyboard..." << status_eol;
	Keyboard::Initialize();
//	terminal << status_good << "Keyboard operational!" << status_eol;

//	terminal << status_pend << "Setting up paging..." << status_eol;
//	beginPaging();
//	terminal << status_fail << "Paging has been manually disabled." << status_eol;

//	terminal << status_good << "Console font is located at: ";
//	writeHex(_mainfont);
//	terminal << status_eol;

//	terminal << status_good << "microNET: boot success (microCORE architecture " << ARCH << ")" << status_eol;

	rect(pos(200, 300), dims(20, 20), 0xFFFF00);
//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();

	serial_msg("\n\nKernel configured.");
}

}
