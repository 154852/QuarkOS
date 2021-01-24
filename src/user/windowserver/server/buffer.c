#include "buffer.h"
#include <windowserver/config.h>
#include <syscall.h>
#include <assertions.h>

Pixel* framebuffer;
Pixel swapbuffer[SUPPORTED_SIZE];

void initialise_buffers() {
	FrameBufferInfo info;
	info.enabled = 1;
	framebuffer_set_state(&info);

	assert(info.width == SUPPORTED_WIDTH);
	assert(info.height == SUPPORTED_HEIGHT);
	// Why 2 times? Is the second half a swapbuffer?
	assert(info.size == SUPPORTED_SIZE * sizeof(unsigned) * 2);

	framebuffer = (Pixel*) info.framebuffer;
}

Pixel* get_swapbuffer() {
	return swapbuffer;
}

Pixel* get_framebuffer() {
	return framebuffer;
}