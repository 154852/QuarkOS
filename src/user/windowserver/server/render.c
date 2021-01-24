#include "render.h"
#include <windowserver/config.h>
#include <windowserver/image.h>
#include "buffer.h"
#include "window.h"
#include "input.h"
#include "windowserver/color.h"

void render_cursor_to_swapbuffer() {
	int x0 = clamp(get_mouse_x(), 1, SUPPORTED_WIDTH - 1);
	int y0 = clamp(get_mouse_y(), 1, SUPPORTED_HEIGHT - 1);

	Pixel* swapbuffer = get_swapbuffer();
	
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			int idx = idx_for_xy(x0 + x, y0 + y);
			swapbuffer[idx] = (x == 0 && y == 0)? COLOR_LIGHTGREY:COLOR_BLACK;
		}
	}
}

void render_window_to_swapbuffer(InternalWindow* window) {
	Pixel* swapbuffer = get_swapbuffer();

	for (int y = 0; y < (int) window->height; y++) {
		int outoffset = idx_for_xy(window->x, y + window->y);
		int offset = idx_for_xyw(0, y, SUPPORTED_WIDTH);
		for (int x = 0; x < (int) window->width; x++) {
			swapbuffer[outoffset + x] = window->raster[offset + x];
		}
	}
}

void render() {
	InternalWindow* windows = get_windows();

	Pixel* swapbuffer = get_swapbuffer();
	Pixel* framebuffer = get_framebuffer();

	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		swapbuffer[idx] = DESKTOP_BACKGROUND;
	}

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present) {
			render_window_to_swapbuffer(&windows[i]);
		}
	}

	render_cursor_to_swapbuffer();
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		framebuffer[idx] = swapbuffer[idx];
	}
}