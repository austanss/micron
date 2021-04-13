#pragma once

#include "types.h"

extern "C" {

namespace cpu {
	namespace idt {
		typedef struct {
			word	baseLow;
			word	selector;
			byte	reservedIst;
			byte	flags;
			word	baseMid;
			dword  	baseHigh;
			dword	reserved;
		} __attribute__((packed)) idt_descriptor;

		/*
		* Interrupt Descriptor Pointer
		*/
		typedef struct {
			word	limit;
			address	base;
		} __attribute__((packed)) idt_ptr;

		void load_idt();
	}
}
}
