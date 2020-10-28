//
// Created by rizet on 10/25/20.
//
#include "gfx.hxx"

GraphicsDriver::GraphicsDriver(uint32_t * _buffer) {
	width = 640;
	height = 480;
	screen = (uint32_t *)0xB000;

	buffer = _buffer;

	// initialize to 0
	for (uint32_t i = 0; i < (width * (height)); i++) {
		buffer[i] = (uint32_t)0;
	}
}

GraphicsDriver::GraphicsDriver() {}

void GraphicsDriver::plot_pixel(int x, int y, uint32_t color) {
	if (x < 0 || width <= x || y < 0 || height <= y)
		return;

	buffer[ width * y + x] = color;
}

void GraphicsDriver::rect(int x, int y, uint32_t w, uint32_t h, uint32_t color) {
	uint32_t i = width* (y - 1);

	// test if the Rectangle will be clipped (will it be fully in the screen or partially)
	if (x >= 0 && x+w < width && y >= 0 && y+h < height) {
		// fully drawn
		i += x + w;
		for (int yy = h; yy > 0; yy--) {
			i += width - w;
			for (int xx = w; xx > 0; xx--) {
				buffer[i++] = color;
			}
		}
	} else {
		// clipped
		for (int yy = y; yy < y+h; yy++) {
			i += width;
			for (int xx = x; xx < x+w; xx++) {
				if (xx >= 0 && xx < width && yy >= 0 && yy < height)
					buffer[i + xx] = color;
			}
		}
	}
}

/**
 * Copy the screen buffer to the screen
 */
void GraphicsDriver::buff() {

	// do the multiply once and test against 0
	for(int i = width * height; i >= 0; i--) {
		screen[i] = buffer[i];
	}



	// clear the buffer once copied
	for(int i = width * height; i >= 0; i--) {
		buffer[i] = 0;
	}
}

void GraphicsDriver::put_char(char ch, int x, int y, uint32_t color) {
	uint32_t px = 0;  // position in the charactor - an 8x8 font = 64 bits
	uint64_t bCh = FONT[ch];

	// check if it will be drawn off screen
	if (x+8 < 0 || x > width || y+8 < 0 || y > height)
		return;

	// test if the charactor will be clipped (will it be fully in the screen or partially)
	if (x >= 0 && x+8 < width && y >= 0 && y+8 < height) {
		// fully in the screen - pre calculate some of the values
		// so there is less going on in the loop
		int i = width * (y - 1) + x + 8;
		int incAmount = width - 8;
		for (int yy = 7; yy >= 0; yy-- ) {
			i += incAmount;
			for (int xx = 7; xx >= 0; xx-- ) {
				// test if a pixel is drawn here
				if ((bCh >> px++) & 1) {
					buffer[i] = color;
				}
				i++;
			}
		}
	} else {
		// partially in the screen
		int xpos = 0;
		int i = width* (y - 1);
		for (int yy = 0; yy < 8; yy++ ) {
			i += width;
			xpos = x;
			for (int xx = 7; xx >= 0; xx-- ) {
				// test if a pixel is drawn here
				if ((bCh >> px++) & 1) {
					// test if the pixel will be off screen
					if (xpos > 0 && xpos < width && yy+y > 0 && yy+y < height)
						buffer[ i + xpos] = color;
				}
				xpos++;
			}
		}
	}
}
