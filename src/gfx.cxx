#include "kernel/memory.hxx"
#include <cstdint>
#include <kernel/gfx.hxx>

uint32_t *buffer;

uint32_t *screen = (uint32_t *)gop.framebuffer_base_addr;

uint32_t width = gop.x_resolution;

uint32_t height = gop.y_resolution;

Framebuffer gop;

positional_point pos(uint32_t x, uint32_t y) {

	positional_point posi;
	posi.x = x;
	posi.y = y;
	return posi;
}

dimensions dims(uint32_t w, uint32_t h) {

	dimensions dimens;
	dimens.w = w;
	dimens.h = h;
	return dimens;
}

void rect(positional_point posi, dimensions dimens, uint32_t color) {

	positional_point rect_relative_center = rect_center(pos(0, 0), pos(dimens.w, dimens.h));

	posi.x -= rect_relative_center.x;
	posi.y -= rect_relative_center.y;

	positional_point rect_absolute_center = pos(posi.x + rect_relative_center.x, posi.y + rect_relative_center.y);

	for (uint32_t xx = (rect_absolute_center.x - (dimens.w / 2)); (xx <= (rect_absolute_center.x + (dimens.w / 2))) && (xx < width); xx++)
		for (uint32_t yy = (rect_absolute_center.y - (dimens.h / 2)); (yy <= rect_absolute_center.y + (dimens.h / 2)) && (yy < height); yy++)
			plot_pixel(pos(xx, yy), color);

	buff();
}

void buff() 
{
	memcpy((void *)gop.framebuffer_base_addr, buffer, gop.x_resolution * gop.y_resolution * 4);
}

void save_screen()
{
	memcpy(buffer, (void *)gop.framebuffer_base_addr, gop.x_resolution * gop.y_resolution * 4);
}

void restore_screen()
{
	memcpy((void *)gop.framebuffer_base_addr, buffer, gop.x_resolution * gop.y_resolution * 4);
}

void plot_pixel(positional_point posi, uint32_t pixel)
{
	*((uint32_t*)(gop.framebuffer_base_addr + 4 * gop.pixels_per_scan_line * posi.y + 4 * posi.x)) = pixel;
}

void plot_pixel_buffer(positional_point posi, uint32_t pixel)
{
	*((uint32_t*)(buffer + 4 * (posi.y * gop.pixels_per_scan_line + posi.x))) = pixel;
}

positional_point rect_center(positional_point posTL, positional_point posBR)
{
	return pos((posTL.x + posBR.x) / 2, (posTL.y + posBR.y) / 2);
}
