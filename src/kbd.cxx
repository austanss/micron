//
// Created by rizet on 10/19/20.
//

#include "kernel/kbd.hxx"
#include "kernel/memory.hxx"
#include "kernel/power.hxx"
#include "kernel/terminal.hxx"
#include "kernel/io.hxx"
#include "kernel/gfx.hxx"

void (*keyboard_event_subscriber[16])();
int keyboard_subscribers = 0;

void io::keyboard::keyboard_event_publisher() 
{
	for (int i = 0; i < keyboard_subscribers; i++)
	{
		keyboard_event_subscriber[i]();
	}
}

void io::keyboard::keyboard_event_subscribe(void (*subscriber_function)())
{
	keyboard_event_subscriber[keyboard_subscribers] = subscriber_function;
	keyboard_subscribers++;
}

void io::keyboard::keyboard_event_unsubscribe(void (*subscriber_function)())
{
	for (int i = 0; i < keyboard_subscribers; i++)
	{
		if (keyboard_event_subscriber[i] == subscriber_function)
		{
			keyboard_event_subscriber[i] = nullptr;
			keyboard_subscribers--;
		}
	}
}

uint8_t* io::keyboard::char_buffer;

void fn_handler_f1  ()  {
	memory::operations::memset(gfx::buffer, 0xFF, gfx::gop.framebuffer_size);
	gfx::screen::buff();
}
void fn_handler_f2  ()  { 
	cpu::power::restart_warm();
}

void fn_handler_f3  ()  { 
	gfx::screen::buff();
}

bool main_screen = true;

void fn_handler_f4  ()  { 
	if (main_screen)
	{
		gfx::screen::save_screen();
		terminal::instance().clear();
	}
	else 
	{
		gfx::screen::restore_screen();
		gfx::screen::save_screen();
	}
	
	main_screen = !main_screen;	
}
void fn_handler_f5  ()  { }
void fn_handler_f6  ()  { }
void fn_handler_f7  ()  { }
void fn_handler_f8  ()  { }
void fn_handler_f9  ()  { }
void fn_handler_f10 ()  { }
void fn_handler_f11 ()  { }
void fn_handler_f12 ()  { }

char io::keyboard::scan_code_to_char(uint16_t keycode)
{
	if (io::keyboard::caps_lock && io::keyboard::shifted)
	{
		switch (keycode)
		{
			case 0x02: return '!';
			case 0x03: return '@';
			case 0x04: return '#';
			case 0x05: return '$';
			case 0x06: return '%';
			case 0x07: return '^';
			case 0x08: return '&';
			case 0x09: return '*';
			case 0x0A: return '(';
			case 0x0B: return ')';
			case 0x0C: return '_';
			case 0x0D: return '+';
			case 0x0E: return '\b';
			case 0x0F: return '\t';
			case 0x10: return 'q';
			case 0x11: return 'w';
			case 0x12: return 'e';
			case 0x13: return 'r';
			case 0x14: return 't';
			case 0x15: return 'y';
			case 0x16: return 'u';
			case 0x17: return 'i';
			case 0x18: return 'o';
			case 0x19: return 'p';
			case 0x1A: return '{';
			case 0x1B: return '}';
			case 0x1C: return '\n';
			case 0x1D: io::keyboard::ctrl_down = true; return 0;
			case 0x1E: return 'a';
			case 0x1F: return 's';
			case 0x20: return 'd';
			case 0x21: return 'f';
			case 0x22: return 'g';
			case 0x23: return 'h';
			case 0x24: return 'j';
			case 0x25: return 'k';
			case 0x26: return 'l';
			case 0x27: return ':';
			case 0x28: return '"';
			case 0x29: return '~';
			case 0x2A: io::keyboard::shifted = true; return 0;
			case 0x2B: return '|';
			case 0x2C: return 'z';
			case 0x2D: return 'x';
			case 0x2E: return 'c';
			case 0x2F: return 'v';
			case 0x30: return 'b';
			case 0x31: return 'n';
			case 0x32: return 'm';
			case 0x33: return '<';
			case 0x34: return '>';
			case 0x35: return '?';
			case 0x36: io::keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: io::keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: io::keyboard::caps_lock = !io::keyboard::alt_down; return 0;
			case 0x3B: fn_handler_f1();  return 0;
			case 0x3C: fn_handler_f2();  return 0;
			case 0x3D: fn_handler_f3();  return 0;
			case 0x3E: fn_handler_f4();  return 0;
			case 0x3F: fn_handler_f5();  return 0;
			case 0x40: fn_handler_f6();  return 0;
			case 0x41: fn_handler_f7();  return 0;
			case 0x42: fn_handler_f8();  return 0;
			case 0x43: fn_handler_f9();  return 0;
			case 0x44: fn_handler_f10(); return 0;
			case 0x45: io::keyboard::num_lock = !io::keyboard::num_lock; return 0;
			case 0x46: io::keyboard::scroll_lock = !io::keyboard::scroll_lock; return 0;
			case 0x47: return '7';
			case 0x48: return '8';
			case 0x49: return '9';
			case 0x4A: return '-';
			case 0x4B: return '4';
			case 0x4C: return '5';
			case 0x4D: return '6';
			case 0x4E: return '+';
			case 0x4F: return '1';
			case 0x50: return '2';
			case 0x51: return '3';
			case 0x52: return '0';
			case 0x53: return '.';
			case 0x57: fn_handler_f1(); return 0;
			case 0x58: fn_handler_f2(); return 0;
			case 0x9D: io::keyboard::ctrl_down = false;
			case 0xAA: io::keyboard::shifted = false;
			case 0xB6: io::keyboard::shifted = false;
			case 0xB8: io::keyboard::alt_down = false;
			case 0xE0: return scan_code_to_char(inb(0x60));

			default: return 0;
		}
	}
	else if (io::keyboard::caps_lock && !io::keyboard::shifted)
	{
		switch (keycode)
		{
			case 0x02: return '1';
			case 0x03: return '2';
			case 0x04: return '3';
			case 0x05: return '4';
			case 0x06: return '5';
			case 0x07: return '6';
			case 0x08: return '7';
			case 0x09: return '8';
			case 0x0A: return '9';
			case 0x0B: return '0';
			case 0x0C: return '-';
			case 0x0D: return '=';
			case 0x0E: return '\b';
			case 0x0F: return '\t';
			case 0x10: return 'Q';
			case 0x11: return 'W';
			case 0x12: return 'E';
			case 0x13: return 'R';
			case 0x14: return 'T';
			case 0x15: return 'Y';
			case 0x16: return 'U';
			case 0x17: return 'I';
			case 0x18: return 'O';
			case 0x19: return 'P';
			case 0x1A: return '[';
			case 0x1B: return ']';
			case 0x1C: return '\n';
			case 0x1D: io::keyboard::ctrl_down = true; return 0;
			case 0x1E: return 'A';
			case 0x1F: return 'S';
			case 0x20: return 'D';
			case 0x21: return 'F';
			case 0x22: return 'G';
			case 0x23: return 'H';
			case 0x24: return 'J';
			case 0x25: return 'K';
			case 0x26: return 'L';
			case 0x27: return ';';
			case 0x28: return '\'';
			case 0x29: return '`';
			case 0x2A: io::keyboard::shifted = true; return 0;
			case 0x2B: return '\\';
			case 0x2C: return 'Z';
			case 0x2D: return 'X';
			case 0x2E: return 'C';
			case 0x2F: return 'V';
			case 0x30: return 'B';
			case 0x31: return 'N';
			case 0x32: return 'M';
			case 0x33: return ',';
			case 0x34: return '.';
			case 0x35: return '/';
			case 0x36: io::keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: io::keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: io::keyboard::caps_lock = !io::keyboard::alt_down; return 0;
			case 0x3B: fn_handler_f1();  return 0;
			case 0x3C: fn_handler_f2();  return 0;
			case 0x3D: fn_handler_f3();  return 0;
			case 0x3E: fn_handler_f4();  return 0;
			case 0x3F: fn_handler_f5();  return 0;
			case 0x40: fn_handler_f6();  return 0;
			case 0x41: fn_handler_f7();  return 0;
			case 0x42: fn_handler_f8();  return 0;
			case 0x43: fn_handler_f9();  return 0;
			case 0x44: fn_handler_f10(); return 0;
			case 0x45: io::keyboard::num_lock = !io::keyboard::num_lock; return 0;
			case 0x46: io::keyboard::scroll_lock = !io::keyboard::scroll_lock; return 0;
			case 0x47: return '7';
			case 0x48: return '8';
			case 0x49: return '9';
			case 0x4A: return '-';
			case 0x4B: return '4';
			case 0x4C: return '5';
			case 0x4D: return '6';
			case 0x4E: return '+';
			case 0x4F: return '1';
			case 0x50: return '2';
			case 0x51: return '3';
			case 0x52: return '0';
			case 0x53: return '.';
			case 0x57: fn_handler_f1(); return 0;
			case 0x58: fn_handler_f2(); return 0;
			case 0x9D: io::keyboard::ctrl_down = false;
			case 0xAA: io::keyboard::shifted = false;
			case 0xB6: io::keyboard::shifted = false;
			case 0xB8: io::keyboard::alt_down = false;
			case 0xE0: return scan_code_to_char(inb(0x60));

			default: return 0;
		}
	}
	else if (io::keyboard::shifted && !io::keyboard::caps_lock)
	{
		switch (keycode)
		{
			case 0x02: return '!';
			case 0x03: return '@';
			case 0x04: return '#';
			case 0x05: return '$';
			case 0x06: return '%';
			case 0x07: return '^';
			case 0x08: return '&';
			case 0x09: return '*';
			case 0x0A: return '(';
			case 0x0B: return ')';
			case 0x0C: return '_';
			case 0x0D: return '+';
			case 0x0E: return '\b';
			case 0x0F: return '\t';
			case 0x10: return 'Q';
			case 0x11: return 'W';
			case 0x12: return 'E';
			case 0x13: return 'R';
			case 0x14: return 'T';
			case 0x15: return 'Y';
			case 0x16: return 'U';
			case 0x17: return 'I';
			case 0x18: return 'O';
			case 0x19: return 'P';
			case 0x1A: return '{';
			case 0x1B: return '}';
			case 0x1C: return '\n';
			case 0x1D: io::keyboard::ctrl_down = true; return 0;
			case 0x1E: return 'A';
			case 0x1F: return 'S';
			case 0x20: return 'D';
			case 0x21: return 'F';
			case 0x22: return 'G';
			case 0x23: return 'H';
			case 0x24: return 'J';
			case 0x25: return 'K';
			case 0x26: return 'L';
			case 0x27: return ':';
			case 0x28: return '"';
			case 0x29: return '~';
			case 0x2A: io::keyboard::shifted = true; return 0;
			case 0x2B: return '|';
			case 0x2C: return 'Z';
			case 0x2D: return 'X';
			case 0x2E: return 'C';
			case 0x2F: return 'V';
			case 0x30: return 'B';
			case 0x31: return 'N';
			case 0x32: return 'M';
			case 0x33: return '<';
			case 0x34: return '>';
			case 0x35: return '?';
			case 0x36: io::keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: io::keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: io::keyboard::caps_lock = !io::keyboard::alt_down; return 0;
			case 0x3B: fn_handler_f1();  return 0;
			case 0x3C: fn_handler_f2();  return 0;
			case 0x3D: fn_handler_f3();  return 0;
			case 0x3E: fn_handler_f4();  return 0;
			case 0x3F: fn_handler_f5();  return 0;
			case 0x40: fn_handler_f6();  return 0;
			case 0x41: fn_handler_f7();  return 0;
			case 0x42: fn_handler_f8();  return 0;
			case 0x43: fn_handler_f9();  return 0;
			case 0x44: fn_handler_f10(); return 0;
			case 0x45: io::keyboard::num_lock = !io::keyboard::num_lock; return 0;
			case 0x46: io::keyboard::scroll_lock = !io::keyboard::scroll_lock; return 0;
			case 0x47: return '7';
			case 0x48: return '8';
			case 0x49: return '9';
			case 0x4A: return '-';
			case 0x4B: return '4';
			case 0x4C: return '5';
			case 0x4D: return '6';
			case 0x4E: return '+';
			case 0x4F: return '1';
			case 0x50: return '2';
			case 0x51: return '3';
			case 0x52: return '0';
			case 0x53: return '.';
			case 0x57: fn_handler_f1(); return 0;
			case 0x58: fn_handler_f2(); return 0;
			case 0x9D: io::keyboard::ctrl_down = false;
			case 0xAA: io::keyboard::shifted = false;
			case 0xB6: io::keyboard::shifted = false;
			case 0xB8: io::keyboard::alt_down = false;
			case 0xE0: return scan_code_to_char(inb(0x60));

			default: return 0;
		}
	}
	else 
	{
		switch (keycode)
		{
			case 0x02: return '1';
			case 0x03: return '2';
			case 0x04: return '3';
			case 0x05: return '4';
			case 0x06: return '5';
			case 0x07: return '6';
			case 0x08: return '7';
			case 0x09: return '8';
			case 0x0A: return '9';
			case 0x0B: return '0';
			case 0x0C: return '-';
			case 0x0D: return '=';
			case 0x0E: return '\b';
			case 0x0F: return '\t';
			case 0x10: return 'q';
			case 0x11: return 'w';
			case 0x12: return 'e';
			case 0x13: return 'r';
			case 0x14: return 't';
			case 0x15: return 'y';
			case 0x16: return 'u';
			case 0x17: return 'i';
			case 0x18: return 'o';
			case 0x19: return 'p';
			case 0x1A: return '[';
			case 0x1B: return ']';
			case 0x1C: return '\n';
			case 0x1D: io::keyboard::ctrl_down = true; return 0;
			case 0x1E: return 'a';
			case 0x1F: return 's';
			case 0x20: return 'd';
			case 0x21: return 'f';
			case 0x22: return 'g';
			case 0x23: return 'h';
			case 0x24: return 'j';
			case 0x25: return 'k';
			case 0x26: return 'l';
			case 0x27: return ';';
			case 0x28: return '\'';
			case 0x29: return '`';
			case 0x2A: io::keyboard::shifted = true; return 0;
			case 0x2B: return '\\';
			case 0x2C: return 'z';
			case 0x2D: return 'x';
			case 0x2E: return 'c';
			case 0x2F: return 'v';
			case 0x30: return 'b';
			case 0x31: return 'n';
			case 0x32: return 'm';
			case 0x33: return ',';
			case 0x34: return '.';
			case 0x35: return '/';
			case 0x36: io::keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: io::keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: io::keyboard::caps_lock = !io::keyboard::alt_down; return 0;
			case 0x3B: fn_handler_f1();  return 0;
			case 0x3C: fn_handler_f2();  return 0;
			case 0x3D: fn_handler_f3();  return 0;
			case 0x3E: fn_handler_f4();  return 0;
			case 0x3F: fn_handler_f5();  return 0;
			case 0x40: fn_handler_f6();  return 0;
			case 0x41: fn_handler_f7();  return 0;
			case 0x42: fn_handler_f8();  return 0;
			case 0x43: fn_handler_f9();  return 0;
			case 0x44: fn_handler_f10(); return 0;
			case 0x45: io::keyboard::num_lock = !io::keyboard::num_lock; return 0;
			case 0x46: io::keyboard::scroll_lock = !io::keyboard::scroll_lock; return 0;
			case 0x47: return '7';
			case 0x48: return '8';
			case 0x49: return '9';
			case 0x4A: return '-';
			case 0x4B: return '4';
			case 0x4C: return '5';
			case 0x4D: return '6';
			case 0x4E: return '+';
			case 0x4F: return '1';
			case 0x50: return '2';
			case 0x51: return '3';
			case 0x52: return '0';
			case 0x53: return '.';
			case 0x57: fn_handler_f1(); return 0;
			case 0x58: fn_handler_f2(); return 0;
			case 0x9D: io::keyboard::ctrl_down = false;
			case 0xAA: io::keyboard::shifted = false;
			case 0xB6: io::keyboard::shifted = false;
			case 0xB8: io::keyboard::alt_down = false;
			case 0xE0: return scan_code_to_char(inb(0x60));

			default: return 0;
		}
	}
}

bool io::keyboard::shifted;
bool io::keyboard::ctrl_down;
bool io::keyboard::alt_down;
bool io::keyboard::fn_down;
bool io::keyboard::caps_lock;
bool io::keyboard::scroll_lock;
bool io::keyboard::num_lock;

void io::keyboard::init()
{
	io::keyboard::num_lock = false;
	io::keyboard::caps_lock = false;
	io::keyboard::scroll_lock = false;
	io::keyboard::alt_down = false;
	io::keyboard::ctrl_down = false;
	io::keyboard::fn_down = false;
	io::pic::irq_unmask(1);
	char_buffer = (uint8_t *)memory::allocation::malloc(buffer_size);
}
