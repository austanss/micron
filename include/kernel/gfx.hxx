//
// Created by rizet on 10/25/20.
//
#pragma once
#include <stdint.h>
#include "kernel/boot.hxx"

namespace gfx {

	extern uint32_t*					buffer;
	extern boot::gop_framebuffer 		gop;

	namespace fonts {
		uint64_t						get_character_font(char c);
	}
	namespace shapes {
		typedef struct s_pos_point {
			uint32_t x;
			uint32_t y;
		} positional_point;

		typedef struct s_dimensions {
			uint32_t w;
			uint32_t h;
		} dimensions;
		positional_point 				pos(uint32_t x, uint32_t y);
		dimensions 						dims(uint32_t w, uint32_t h);
		void 							rect(positional_point posi, dimensions dimens, uint32_t color);
		positional_point 				rect_center(positional_point posTL, positional_point posBR);
	}
	namespace screen {

		inline void plot_pixel(gfx::shapes::positional_point posi, uint32_t pixel)
		{
			gop.framebuffer_base[gop.x_resolution * posi.y + posi.x] = pixel;
		}

		inline void plot_pixel_buffer(gfx::shapes::positional_point posi, uint32_t pixel)
		{
			gop.framebuffer_base[gop.x_resolution * posi.y + posi.x] = pixel;
		}

		void 							save_screen();
		void 							restore_screen();
		void 							buff();
	}
};
