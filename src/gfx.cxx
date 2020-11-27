#include <kernel/gfx.hxx>

uint32_t *buffer = (uint32_t *)0xB00B1E5;

uint32_t *screen = (uint32_t *)gop.framebuffer_base_addr;

uint32_t width = gop.x_resolution;

uint32_t height = gop.y_resolution;

Framebuffer gop;

void rect(int x, int y, uint32_t w, uint32_t h, uint32_t color) {

	for (unsigned int xx = x; (xx <= (x + w)) && (xx < width); xx++)
		for (unsigned int yy = y; (yy <= (y + h)) && (yy < height); yy++)
			plot_pixel(xx, yy, color);

	buff();
}

void buff() {

	for (unsigned int i = 0; i < (width * height); i++)
	{
		if (screen[i] != buffer[i])
			screen[i] = buffer[i];
	}
}

void plot_pixel(int x, int y, uint32_t pixel)
{
	*((uint32_t*)(gop.framebuffer_base_addr + 4 * gop.pixels_per_scan_line * y + 4 * x)) = pixel;
}

void plot_pixel_buffer(int x, int y, uint32_t pixel)
{
	*(buffer + 4 * gop.pixels_per_scan_line * y + 4 * x) = pixel;
}

