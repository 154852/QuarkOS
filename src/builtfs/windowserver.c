#include "syscall.h"
#include <stdio.h>
#include <assertions.h>
#include <string.h>

#define SUPPORTED_SIZE 0xc0000
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

int main() {
	info.enabled = 1;
	framebuffer_set_state(&info);

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
	blit();

	hang;
	return 0;
}
