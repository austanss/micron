//
// Created by rizet on 10/19/20.
//

#include "kernel/kbd.hxx"
#include "kernel/terminal.hxx"

void FnHandlerF1  ()  { }
void FnHandlerF2  ()  { }
void FnHandlerF3  ()  { }
void FnHandlerF4  ()  { }
void FnHandlerF5  ()  { }
void FnHandlerF6  ()  { }
void FnHandlerF7  ()  { }
void FnHandlerF8  ()  { }
void FnHandlerF9  ()  { }
void FnHandlerF10 ()  { }
void FnHandlerF11 ()  { }
void FnHandlerF12 ()  { }

char getChar(uint8_t keycode)
{
	if (Keyboard::caps_lock && Keyboard::shifted)
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
			case 0x1D: Keyboard::ctrl_down = true; return 0;
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
			case 0x2A: Keyboard::shifted = true; return 0;
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
			case 0x36: Keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: Keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: Keyboard::caps_lock = !Keyboard::alt_down; return 0;
			case 0x3B: FnHandlerF1();  return 0;
			case 0x3C: FnHandlerF2();  return 0;
			case 0x3D: FnHandlerF3();  return 0;
			case 0x3E: FnHandlerF4();  return 0;
			case 0x3F: FnHandlerF5();  return 0;
			case 0x40: FnHandlerF6();  return 0;
			case 0x41: FnHandlerF7();  return 0;
			case 0x42: FnHandlerF8();  return 0;
			case 0x43: FnHandlerF9();  return 0;
			case 0x44: FnHandlerF10(); return 0;
			case 0x45: Keyboard::num_lock = !Keyboard::num_lock; return 0;
			case 0x46: Keyboard::scroll_lock = !Keyboard::scroll_lock; return 0;
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
			case 0x57: FnHandlerF11(); return 0;
			case 0x58: FnHandlerF12(); return 0;
			case 0x9D: Keyboard::ctrl_down = false;
			case 0xAA: Keyboard::shifted = false;
			case 0xB6: Keyboard::shifted = false;
			case 0xB8: Keyboard::alt_down = false;

			default: return 0;
		}
	}
	else if (Keyboard::caps_lock && !Keyboard::shifted)
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
			case 0x1D: Keyboard::ctrl_down = true; return 0;
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
			case 0x2A: Keyboard::shifted = true; return 0;
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
			case 0x36: Keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: Keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: Keyboard::caps_lock = !Keyboard::alt_down; return 0;
			case 0x3B: FnHandlerF1();  return 0;
			case 0x3C: FnHandlerF2();  return 0;
			case 0x3D: FnHandlerF3();  return 0;
			case 0x3E: FnHandlerF4();  return 0;
			case 0x3F: FnHandlerF5();  return 0;
			case 0x40: FnHandlerF6();  return 0;
			case 0x41: FnHandlerF7();  return 0;
			case 0x42: FnHandlerF8();  return 0;
			case 0x43: FnHandlerF9();  return 0;
			case 0x44: FnHandlerF10(); return 0;
			case 0x45: Keyboard::num_lock = !Keyboard::num_lock; return 0;
			case 0x46: Keyboard::scroll_lock = !Keyboard::scroll_lock; return 0;
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
			case 0x57: FnHandlerF11(); return 0;
			case 0x58: FnHandlerF12(); return 0;
			case 0x9D: Keyboard::ctrl_down = false;
			case 0xAA: Keyboard::shifted = false;
			case 0xB6: Keyboard::shifted = false;
			case 0xB8: Keyboard::alt_down = false;

			default: return 0;
		}
	}
	else if (Keyboard::shifted && !Keyboard::caps_lock)
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
			case 0x1D: Keyboard::ctrl_down = true; return 0;
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
			case 0x2A: Keyboard::shifted = true; return 0;
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
			case 0x36: Keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: Keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: Keyboard::caps_lock = !Keyboard::alt_down; return 0;
			case 0x3B: FnHandlerF1();  return 0;
			case 0x3C: FnHandlerF2();  return 0;
			case 0x3D: FnHandlerF3();  return 0;
			case 0x3E: FnHandlerF4();  return 0;
			case 0x3F: FnHandlerF5();  return 0;
			case 0x40: FnHandlerF6();  return 0;
			case 0x41: FnHandlerF7();  return 0;
			case 0x42: FnHandlerF8();  return 0;
			case 0x43: FnHandlerF9();  return 0;
			case 0x44: FnHandlerF10(); return 0;
			case 0x45: Keyboard::num_lock = !Keyboard::num_lock; return 0;
			case 0x46: Keyboard::scroll_lock = !Keyboard::scroll_lock; return 0;
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
			case 0x57: FnHandlerF11(); return 0;
			case 0x58: FnHandlerF12(); return 0;
			case 0x9D: Keyboard::ctrl_down = false;
			case 0xAA: Keyboard::shifted = false;
			case 0xB6: Keyboard::shifted = false;
			case 0xB8: Keyboard::alt_down = false;

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
			case 0x1D: Keyboard::ctrl_down = true; return 0;
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
			case 0x2A: Keyboard::shifted = true; return 0;
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
			case 0x36: Keyboard::shifted = true; return 0;
			case 0x37: return '*';
			case 0x38: Keyboard::alt_down = true; return 0;
			case 0x39: return ' ';
			case 0x3A: Keyboard::caps_lock = !Keyboard::alt_down; return 0;
			case 0x3B: FnHandlerF1();  return 0;
			case 0x3C: FnHandlerF2();  return 0;
			case 0x3D: FnHandlerF3();  return 0;
			case 0x3E: FnHandlerF4();  return 0;
			case 0x3F: FnHandlerF5();  return 0;
			case 0x40: FnHandlerF6();  return 0;
			case 0x41: FnHandlerF7();  return 0;
			case 0x42: FnHandlerF8();  return 0;
			case 0x43: FnHandlerF9();  return 0;
			case 0x44: FnHandlerF10(); return 0;
			case 0x45: Keyboard::num_lock = !Keyboard::num_lock; return 0;
			case 0x46: Keyboard::scroll_lock = !Keyboard::scroll_lock; return 0;
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
			case 0x57: FnHandlerF11(); return 0;
			case 0x58: FnHandlerF12(); return 0;
			case 0x9D: Keyboard::ctrl_down = false;
			case 0xAA: Keyboard::shifted = false;
			case 0xB6: Keyboard::shifted = false;
			case 0xB8: Keyboard::alt_down = false;

			default: return 0;
		}
	}
}

bool Keyboard::shifted;
bool Keyboard::ctrl_down;
bool Keyboard::alt_down;
bool Keyboard::fn_down;
bool Keyboard::caps_lock;
bool Keyboard::scroll_lock;
bool Keyboard::num_lock;

void Keyboard::Initialize()
{
	Keyboard::num_lock = false;
	Keyboard::caps_lock = false;
	Keyboard::scroll_lock = false;
	Keyboard::alt_down = false;
	Keyboard::ctrl_down = false;
	Keyboard::fn_down = false;
}
