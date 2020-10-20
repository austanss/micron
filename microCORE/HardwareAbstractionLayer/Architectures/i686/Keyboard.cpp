//
// Created by rizet on 10/19/20.
//

#include "Keyboard.h"

char getChar(uint8_t keycode)
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
		case 0x1D:
		case 0x1E: return 'a';
		case 0x1F: return 's';
		case 0x20: return 'd';
		case 0x24: return 'j';
		case 0x28: return '\'';

		default: return 0;
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
