//
// Created by rizet on 10/19/20.
//

#include "kbd.h"
#include "../8259/pic.h"
#include "../io.h"
#include "../evsys/evsys.h"

#define EVSYS_KEYBOARD_EVID 0x5286

extern "C" void kbd_irq_handler()
{
	word scan_code = io::inb(0x60);
	if (scan_code == 0xE0)
	{
		scan_code <<= 8;
		scan_code |= io::inb(0x60);
	}

	struct io::keyboard::keyboard_packet kbpacket;

	if ((scan_code >> 8) == 0xE0)
	{
		scan_code &= 0x00FF;
		scan_code -= 0x10;
		if (scan_code < 0x80)
		{	
			kbpacket.release_or_press = 1;
		}
		else
		{
			kbpacket.release_or_press = 0;
			scan_code -= 0x80;
		}
		scan_code += 0x60;
		kbpacket.key_code = (scan_code & 0x00FF);
	}
	else
	{
		if (scan_code < 0x80)
		{	
			kbpacket.release_or_press = 1;
			kbpacket.key_code = scan_code;
		}
		else
		{
			kbpacket.release_or_press = 0;
			kbpacket.key_code = scan_code - 0x80;
		}
	}

	if (kbpacket.key_code == 0x1D || kbpacket.key_code == 0x8D)
		io::keyboard::ctrl_down = kbpacket.release_or_press;

	if (kbpacket.key_code == 0x38 || kbpacket.key_code == 0xA8)
		io::keyboard::alt_down = kbpacket.release_or_press;

	if (kbpacket.key_code == 0x2A || kbpacket.key_code == 0x36)
		io::keyboard::shifted = kbpacket.release_or_press;

	if (kbpacket.key_code == 0x3A && kbpacket.release_or_press)
		io::keyboard::caps_lock =! io::keyboard::caps_lock;	

	if (kbpacket.key_code == 0x45 && kbpacket.release_or_press)
		io::keyboard::num_lock =! io::keyboard::num_lock;	

	if (kbpacket.key_code == 0x46 && kbpacket.release_or_press)
		io::keyboard::scroll_lock =! io::keyboard::scroll_lock;	

	kbpacket.alt_down = io::keyboard::alt_down;
	kbpacket.ctrl_down = io::keyboard::ctrl_down;
	kbpacket.fn_down = io::keyboard::fn_down;
	kbpacket.shifted = io::keyboard::shifted;
	kbpacket.caps_lock = io::keyboard::caps_lock;
	kbpacket.num_lock = io::keyboard::num_lock;
	kbpacket.scroll_lock = io::keyboard::scroll_lock;
	
	sys::evsys::fire_event(EVSYS_KEYBOARD_EVID, &kbpacket, sizeof(io::keyboard::keyboard_packet));
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
}
