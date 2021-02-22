//
// Created by rizet on 10/25/20.
//
#pragma once
#include "types.hxx"
#include "start/boot.hxx"

namespace gfx {

	extern uint32*								buffer;
	extern stivale_framebuffer			 		gop;

	namespace fonts {
		uint64						get_character_font(char c);
	}
	namespace shapes {
		typedef struct s_pos_point {
			uint32 x;
			uint32 y;
		} positional_point;

		typedef struct s_dimensions {
			uint32 w;
			uint32 h;
		} dimensions;
		positional_point 				pos(uint32 x, uint32 y);
		dimensions 						dims(uint32 w, uint32 h);
		void 							rect(positional_point posi, dimensions dimens, uint32 color);
		positional_point 				rect_center(positional_point posTL, positional_point posBR);
	}
	namespace screen {

		static inline void plot_pixel(gfx::shapes::positional_point posi, uint32 pixel)
		{
			((dword *)gop.framebuffer_addr)[gop.framebuffer_width * posi.y + posi.x] = pixel;
		}

		static inline void plot_pixel_buffer(gfx::shapes::positional_point posi, uint32 pixel)
		{
			((dword *)gop.framebuffer_addr)[gop.framebuffer_width * posi.y + posi.x] = pixel;
		}

		void 							save_screen();
		void 							restore_screen();
		void 							buff();
	}
}
