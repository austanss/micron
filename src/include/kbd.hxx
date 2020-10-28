//
// Created by rizet on 10/19/20.
//

#ifndef MICROCORE_KBD_HXX
#define MICROCORE_KBD_HXX
#include <stdint.h>

char getChar(uint8_t keycode);

class Keyboard {

public:
	static void Initialize();
	static bool shifted;
	static bool ctrl_down;
	static bool alt_down;
	static bool fn_down;
	static bool caps_lock;
	static bool scroll_lock;
	static bool num_lock;
};

#endif // MICROCORE_KBD_HXX
