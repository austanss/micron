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
		void		keyboard_event_publisher();
		void		keyboard_event_subscribe(void (*subscriber_function)());
		void		keyboard_event_unsubscribe(void (*subscriber_function)());
		extern bool shifted;
		extern bool ctrl_down;
		extern bool alt_down;
		extern bool fn_down;
		extern bool caps_lock;
		extern bool scroll_lock;
		extern bool num_lock;
		constexpr uint16_t buffer_size = 0x400;
		extern uint8_t* char_buffer;
	}
	
};

#endif // MICROCORE_KBD_HXX
