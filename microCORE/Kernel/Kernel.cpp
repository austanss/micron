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
	terminal << "\n";

	terminal.staticLogo = true;

	terminal << Terminal::Status << "Initializing keyboard..." << Terminal::EOL;
	Keyboard::Initialize();
	terminal << Terminal::Good << "Keyboard operational!" << Terminal::EOL;

	terminal << Terminal::Status << "Setting up paging..." << Terminal::EOL;
//	MemoryManagement::beginPaging();
	terminal << Terminal::Fail << "Paging has been manually disabled by the developer." << Terminal::EOL;

	terminal << Terminal::Good << "Console font is located at: ";
	writeHex(_mainfont);
	terminal << Terminal::EOL;

	terminal << Terminal::Good << "microNET: boot success (microCORE architecture " << ARCH << ")" << Terminal::EOL;

//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();
}

}
