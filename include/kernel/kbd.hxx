//
// Created by rizet on 10/19/20.
//

#ifndef MICROCORE_KBD_HXX
#define MICROCORE_KBD_HXX
#include <stdint.h>


namespace io {

	namespace keyboard {
		void 		init();
		char 		scan_code_to_char(uint8_t keycode);
		extern bool shifted;
		extern bool ctrl_down;
		extern bool alt_down;
		extern bool fn_down;
		extern bool caps_lock;
		extern bool scroll_lock;
		extern bool num_lock;
	}
	
};

#endif // MICROCORE_KBD_HXX
