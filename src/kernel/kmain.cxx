#include "kutil.hxx"
#include "terminal.hxx"
#include "Memory.hxx"
#include "microNETlogo.hxx"
#include "Macros.hxx"
#include "Keyboard.hxx"
#include "Fonts.hxx"
#include "io.hxx"

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif

#define OS_CONTINOUS_RAM_START 0x100000

uint32_t framebuffer[640 * 480];

extern "C" {

// global constructors
void ctor_global
// required params
(/* none atm */)
// constructors called
{
	Terminal::init(framebuffer);
	serial_msg("GLOBAL CONSTRUCTORS CALLED");
	serial_msg(0xBA);
}

void kernel_main()
{
	Terminal& terminal = Terminal::instance();
	terminal.setCursor(0, 0);
	terminal << logo;
	terminal << status_eol;

	terminal.staticLogo = true;

	terminal << status_pend << "Initializing keyboard..." << status_eol;
	Keyboard::Initialize();
	terminal << status_good << "Keyboard operational!" << status_eol;

	terminal << status_pend << "Setting up paging..." << status_eol;
//	MemoryManagement::beginPaging();
	terminal << status_fail << "Paging has been manually disabled by the developer." << status_eol;

	terminal << status_good << "Console font is located at: ";
	writeHex(_mainfont);
	terminal << status_eol;

	terminal << status_good << "microNET: boot success (microCORE architecture " << ARCH << ")" << status_eol;

//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();
}

}
