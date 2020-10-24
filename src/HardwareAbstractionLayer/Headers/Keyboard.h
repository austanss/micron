//
// Created by rizet on 10/19/20.
//

#ifndef MICROCORE_KEYBOARD_H
#define MICROCORE_KEYBOARD_H
#include <stdint.h>

char getChar(uint8_t keycode);

class Keyboard {

	static bool shifted;
	static bool ctrl_down;
	static bool alt_down;
	static bool fn_down;
	static bool caps_lock;
	static bool scroll_lock;
	static bool num_lock;

public:
	static void Initialize();
};

#endif // MICROCORE_KEYBOARD_H
