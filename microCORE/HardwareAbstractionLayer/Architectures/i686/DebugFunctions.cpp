#include "Terminal.h"

extern "C"
{

void term_test_post()
{
	Terminal::instance().write(Terminal::Status);
	Terminal::instance().write("Testing terminal functionality, post-kernel\n");
//	Terminal::instance() << Terminal::EOL;
}

void term_test_pre()
{
	Terminal::instance().write(Terminal::Status);
	Terminal::instance().write("Testing terminal functionality, pre-kernel\n");
//	Terminal::instance() << Terminal::EOL;
}

}
