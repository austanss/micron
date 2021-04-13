//
// Created by rizet on 10/19/20.
//
#pragma once
#include "types.h"


namespace io {
	namespace keyboard {
		void 		init();
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

		enum key_code {
			null,
			escape,
			one,
			two,
			three,
			four,
			five,
			six,
			seven,
			eight,
			nine,
			zero,
			dash,
			equal,
			backspace,
			tab,
			q,
			w,
			e,
			r,
			t,
			y,
			u,
			i,
			o,
			p,
			open_bracket,
			close_bracket,
			enter,
			left_ctrl,
			a,
			s,
			d,
			f,
			g,
			h,
			j,
			k,
			l,
			semicolon,
			apostrophe,
			backtick,
			left_shift,
			backslash,
			z,
			x,
			c,
			v,
			b,
			n,
			m,
			comma,
			period,
			slash,
			right_shift,
			keypad_star,
			left_alt,
			space,
			caps,
			f1,
			f2,
			f3,
			f4,
			f5,
			f6,
			f7,
			f8,
			f9,
			f10,
			number_lock,
			scrl_lock,
			keypad_seven,
			keypad_eight,
			keypad_nine,
			keypad_dash,
			keypad_four,
			keypad_five,
			keypad_six,
			keypad_plus,
			keypad_one,
			keypad_two,
			keypad_three,
			keypad_zero,
			keypad_period
		};
	}	
}