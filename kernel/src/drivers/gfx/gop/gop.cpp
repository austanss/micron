#include "types.h"
#include "memory/operations.h"
#include "gop.h"

stivale_framebuffer 	gfx::gop;
uint32*					gfx::buffer;

gfx::shapes::positional_point gfx::shapes::pos(uint32 x, uint32 y) {

	gfx::shapes::positional_point posi;
	posi.x = x;
	posi.y = y;
	return posi;
}

gfx::shapes::dimensions gfx::shapes::dims(uint32 w, uint32 h) {

	gfx::shapes::dimensions dimens;
	dimens.w = w;
	dimens.h = h;
	return dimens;
}

void gfx::shapes::rect(gfx::shapes::positional_point posi, gfx::shapes::dimensions dimens, uint32 color) {

	gfx::shapes::positional_point rect_relative_center = gfx::shapes::rect_center(pos(0, 0), pos(dimens.w, dimens.h));

	posi.x -= rect_relative_center.x;
	posi.y -= rect_relative_center.y;

	gfx::shapes::positional_point rect_absolute_center = pos(posi.x + rect_relative_center.x, posi.y + rect_relative_center.y);

	for (uint xx = (rect_absolute_center.x - (dimens.w / 2)); (xx <= (rect_absolute_center.x + (dimens.w / 2))) && (xx < gop.framebuffer_width); xx++)
		for (uint yy = (rect_absolute_center.y - (dimens.h / 2)); (yy <= rect_absolute_center.y + (dimens.h / 2)) && (yy < gop.framebuffer_height); yy++)
			gfx::screen::plot_pixel(gfx::shapes::pos(xx, yy), color);

	gfx::screen::buff();
}

void gfx::screen::buff() 
{
	memory::operations::memcpy((void*)gop.framebuffer_addr, buffer, gop.framebuffer_width * gop.framebuffer_height * 4);
}

void gfx::screen::save_screen()
{
	memory::operations::memcpy(buffer, (void *)gop.framebuffer_addr, gop.framebuffer_width * gop.framebuffer_height * 4);
}

void gfx::screen::restore_screen()
{
	memory::operations::memcpy((void *)gop.framebuffer_addr, buffer, gop.framebuffer_width * gop.framebuffer_height * 4);
}

gfx::shapes::positional_point gfx::shapes::rect_center(gfx::shapes::positional_point posTL, gfx::shapes::positional_point posBR)
{
	return gfx::shapes::pos((posTL.x + posBR.x) / 2, (posTL.y + posBR.y) / 2);
}