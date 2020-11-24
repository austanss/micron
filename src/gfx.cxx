#include <kernel/gfx.hxx>

uint32_t *buffer = (uint32_t *)0xBFFE;

uint32_t *screen = (uint32_t *)gop.framebuffer_base_addr;

uint32_t width = gop.x_resolution;

uint32_t height = gop.y_resolution;

Framebuffer gop;

void rect(int x, int y, uint32_t w, uint32_t h, uint32_t color) {

	uint32_t i = width* (y - 1);

	// test if the Rectangle will be clipped (will it be fully in the screen or partially)
	if (x >= 0 && x+w < width && y >= 0 && y+h < height) {
		// fully drawn
		i += x + w;
		for(int yy = h; yy > 0; yy--) {
			i += width - w;
			for(int xx = w; xx > 0; xx--) {
				buffer[i++] = color;
			}
		}
	} else {
		// clipped
		for(int yy = y; yy < y+h; yy++) {
			i += width;
			for(int xx = x; xx < x+w; xx++) {
				if (xx >= 0 && xx < width && yy >= 0 && yy < height)
					buffer[i + xx] = color;
			}
		}
	}

	buff();
}

void buff() {
	// do the multiply once and test against 0
	for(int i = width * height; i >= 0; i--) {
		screen[i] = buffer[i];
	}



	// clear the buffer once copied
	for(int i = width * height; i >= 0; i--) {
		buffer[i] = 0;
	}
}

void plot_pixel(int x, int y, uint32_t pixel)
{
	*((uint32_t*)(gop.framebuffer_base_addr + 4 * gop.pixels_per_scan_line * y + 4 * x)) = pixel;
}
