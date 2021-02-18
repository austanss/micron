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
		void		keyboard_event_publisher(struct keyboard_packet kbpacket);
		void		keyboard_event_subscribe(void (*subscriber_function)(struct keyboard_packet kbpacket));
		void		keyboard_event_unsubscribe(void (*subscriber_function)(struct keyboard_packet kbpacket));
		struct keyboard_packet {
			unsigned char key_code 	: 8;
			bool release_or_press 	: 1; // release = 0, press = 1
			bool shifted 			: 1;
			bool ctrl_down 			: 1;
			bool alt_down 			: 1;
			bool fn_down 			: 1;
			bool caps_lock 			: 1;
			bool scroll_lock 		: 1;
			bool num_lock 			: 1;
		} __attribute__((packed));
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
