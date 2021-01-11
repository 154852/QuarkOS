#include "syscall.h"
#include <stdio.h>
#include <assertions.h>
#include <string.h>
#include "fontchars.h"

#define SUPPORTED_WIDTH 1024
#define SUPPORTED_HEIGHT 768
#define SUPPORTED_SIZE (SUPPORTED_WIDTH * SUPPORTED_HEIGHT)
#define FULL_SIZE (SUPPORTED_SIZE * sizeof(unsigned))

typedef union {
	struct {
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};

	unsigned raw;
} Pixel;

Pixel data[SUPPORTED_SIZE];
FrameBufferInfo info;
unsigned* framebuffer;

void blit() {
	memcpy(framebuffer, data, FULL_SIZE);
	memset(data, 0, FULL_SIZE);
}

int idx_for_xy(int x, int y) {
	return ((y * info.width) + x);
}

int idx_for_xyw(int x, int y, int w) {
	return ((y * w) + x);
}

void copy_image(int x0, int y0, Pixel* image, int w, int h) {
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int image_idx = idx_for_xyw(x, y, w);
			int framebuffer_idx = idx_for_xy(x + x0, y + y0);

			data[framebuffer_idx] = image[image_idx];
			data[framebuffer_idx].a = 0;
		}
	}
}

int main() {
	info.enabled = 1;
	framebuffer_set_state(&info);

	assert(info.width == SUPPORTED_WIDTH);
	assert(info.height == SUPPORTED_HEIGHT);
	assert(info.size == SUPPORTED_SIZE * sizeof(unsigned) * 2);

	framebuffer = info.framebuffer;

	for (int x = 0; x < info.width; x++) {
		for (int y = 0; y < info.height; y++) {
			int idx = idx_for_xy(x, y);
			unsigned char color = (0xff * x * y) / (info.width * info.height);
			data[idx].r = color;
			data[idx].g = color;
			data[idx].b = color;
		}
	}

	int x = 0;
	copy_image(x, 0, (Pixel*) FONTCHAR_A, FONTCHAR_A_W, FONTCHAR_A_H); x += FONTCHAR_A_W;
	copy_image(x, 0, (Pixel*) FONTCHAR_B, FONTCHAR_B_W, FONTCHAR_B_H); x += FONTCHAR_B_W;
	copy_image(x, 0, (Pixel*) FONTCHAR_C, FONTCHAR_C_W, FONTCHAR_C_H); x += FONTCHAR_C_W;

	copy_image(x, 0, (Pixel*) FONTCHAR_X, FONTCHAR_X_W, FONTCHAR_X_H); x += FONTCHAR_X_W;
	copy_image(x, 0, (Pixel*) FONTCHAR_Y, FONTCHAR_Y_W, FONTCHAR_Y_H); x += FONTCHAR_Y_W;
	copy_image(x, 0, (Pixel*) FONTCHAR_Z, FONTCHAR_Z_W, FONTCHAR_Z_H); x += FONTCHAR_Z_W;

	blit();

	hang;
	return 0;
}
