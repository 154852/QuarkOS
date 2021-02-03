#include "render.h"
#include <windowserver/config.h>
#include <windowserver/image.h>
#include "buffer.h"
#include "window.h"
#include "input.h"
#include "windowserver/color.h"
#include <stdio.h>

static Theme theme;

void set_theme(Theme theme_) {
	theme = theme_;
}

Theme* get_theme() {
	return &theme;
}

void render_window_to_swapbuffer(InternalWindow* window) {
	Pixel* swapbuffer = get_swapbuffer();

	for (int y = 0; y < (int) window->height; y++) {
		if (y + window->y < 0 || y + window->y >= SUPPORTED_HEIGHT) continue;

		int outoffset = idx_for_xy(window->x, y + window->y);
		int offset = idx_for_xyw(0, y, SUPPORTED_WIDTH);
		for (int x = 0; x < (int) window->width; x++) {
			if (x + window->x < 0 || x + window->x >= SUPPORTED_WIDTH) continue;

			if (window->raster[offset + x].a == 0xff) {
				swapbuffer[outoffset + x] = window->raster[offset + x];
			} else if (window->raster[offset + x].a != 0) {
				float frac = (float) window->raster[offset + x].a / (float) 0xff;
				swapbuffer[outoffset + x].r = mix(swapbuffer[outoffset + x].r, window->raster[offset + x].r, frac);
				swapbuffer[outoffset + x].g = mix(swapbuffer[outoffset + x].g, window->raster[offset + x].g, frac);
				swapbuffer[outoffset + x].b = mix(swapbuffer[outoffset + x].b, window->raster[offset + x].b, frac);
			}
		}
	}
}

void render() {
	InternalWindow** windows = get_windows();

	Pixel* swapbuffer = get_swapbuffer();
	Pixel* framebuffer = get_framebuffer();

	theme.render_desktop_background();

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i] && windows[i] != get_focused()) {
			render_window_to_swapbuffer(windows[i]);
		}
	}

	if (get_focused() != 0)
		render_window_to_swapbuffer(get_focused());

	theme.render_cursor();
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		framebuffer[idx] = swapbuffer[idx];
	}
}