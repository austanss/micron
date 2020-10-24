#include "KernelUtil.h"
#include "Terminal.h"
#include "Memory.h"
#include "microNETlogo.h"
#include "Macros.h"
#include "Keyboard.h"
#include "Fonts.h"

#ifndef ARCH
    #define ARCH "$RED!UNKNOWN"
#endif


extern "C" {

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
