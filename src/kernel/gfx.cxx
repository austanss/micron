#include <kernel/gfx.hxx>

void rect(int x, int y, uint32_t w, uint32_t h, uint32_t color) {

	uint64_t *at = (uint64_t *)gop.framebuffer_base_addr;
	uint64_t row, col;

	at += (gop.x_resolution * (y - w / 2) + x - w / 2);

	for (row = 0; row < w / 2; row++) {
		for (col = 0; col < w - row * 2; col++)
			*at++ = color;
		at += (gop.x_resolution - col);
		for (col = 0; col < w - row * 2; col++)
			*at++ = color;
		at += (gop.x_resolution - col + 1);
	}
}