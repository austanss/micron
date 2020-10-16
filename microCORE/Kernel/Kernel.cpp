#include "Terminal.h"
#include "Memory.h"
#include "microNETlogo.h"
#include "microCOREconfig.h"

extern "C" {

void kernel_main()
{
	Terminal& terminal = Terminal::instance();
	terminal.clear();
	terminal.setCursor(0, 0);
	terminal << logo;
	terminal << "\n";

	terminal << Terminal::Status << "Setting up paging..." << Terminal::EOL;
	MemoryManagement::beginPaging();
	terminal << Terminal::Good << "Set up paging!" << Terminal::EOL;


	terminal << Terminal::Good << "Started up microNET (microCORE architecture " << microCORE::Config::Architecture << ")" << Terminal::EOL;
	
//	terminal.clear();
}

}
