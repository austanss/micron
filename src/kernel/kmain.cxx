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
	serial_msg("GLOBAL CONSTRUCTORS CALLED");
	serial_msg(0xBA);
}

void kernel_main(Boot_Info* bootloader_info)
{
	Terminal& terminal = Terminal::instance();

	terminal.setCursor(0, 0);
	terminal << logo;
	terminal << status_eol;

	terminal.staticLogo = true;

	uint64_bytearr ubaraddr;
	ubaraddr.i = bootloader_info->vbe_framebuffer.framebuffer_base_addr;

	for (uint8_t i : ubaraddr.b)
		serial_msg(i);

	rect(120, 120, 30, 40, 0xAA0000AA);

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
