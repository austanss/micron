#pragma once

#include "stdint.h"

extern "C" {

namespace cpu {
	namespace idt {
		typedef struct {
			uint16_t  baseLow;
			uint16_t  selector;
			uint8_t  reservedIst;
			uint8_t  flags;
			uint16_t  baseMid;
			uint32_t  baseHigh;
			uint32_t  reserved;
		} __attribute__((packed)) idt_descriptor;

		/*
		* Interrupt Descriptor Pointer
		*/
		typedef struct {
			uint16_t  limit;
			uint64_t  base;
		} __attribute__((packed)) idt_ptr;

		void loadIDT();
		void initIDT();
	}
}
}
