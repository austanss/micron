#include "Terminal.h"
#include "Memory.h"
#include "microNETlogo.h"
#include "Macros.h"

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

	terminal << Terminal::Status << "Setting up paging..." << Terminal::EOL;
	MemoryManagement::beginPaging();
	terminal << Terminal::Good << "Set up paging!" << Terminal::EOL;

	terminal << Terminal::Good << "microNET: boot success (microCORE architecture " << ARCH << ")" << Terminal::EOL;

//	DO(8) {
//	    terminal << "[TEST]\n";
//	}

	//	terminal.clear();
}

}
