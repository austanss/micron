#include "Terminal.h"

extern "C"
{

void term_test_post()
{
	Terminal::instance().write(status_pend);
	Terminal::instance().write("Testing terminal functionality, post-kernel\n");
//	Terminal::instance() << status_eol;
}

void term_test_pre()
{
	Terminal::instance().write(status_pend);
	Terminal::instance().write("Testing terminal functionality, pre-kernel\n");
//	Terminal::instance() << status_eol;
}

}
