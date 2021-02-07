#include "render.h"
#include <windowserver/config.h>
#include <windowserver/image.h>
#include "assertions.h"
#include "buffer.h"
#include "syscall.h"
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
				unsigned char a = window->raster[offset + x].a;

				swapbuffer[outoffset + x].r = mixi(swapbuffer[outoffset + x].r, window->raster[offset + x].r, a);
				swapbuffer[outoffset + x].g = mixi(swapbuffer[outoffset + x].g, window->raster[offset + x].g, a);
				swapbuffer[outoffset + x].b = mixi(swapbuffer[outoffset + x].b, window->raster[offset + x].b, a);
			}
		}
	}
}

#define TIME_MAX (24 * 60 * 60)
#define TIME_STOP 10

#define DEBUG_FPS

#ifdef DEBUG_FPS
int start = -1;
int frames = 0;
#endif
void render() {
#ifdef DEBUG_FPS
	if (frames % 3 == 0) {
		FullTime now; 
		get_full_time(&now);

		if (start == -1) {
			start = now.hour * 3600 + now.minute * 60 + now.second;
		} else {
			int stamp = now.hour * 3600 + now.minute * 60 + now.second;
			int diff = 0;
			if (stamp < start) diff = (TIME_MAX - start) + stamp;
			else diff = stamp - start;

			if (diff > TIME_STOP) {
				debugf("[WindowServer] Rendered %d frames in %d seconds (%ffps)\n", frames, diff, (float) frames / (float) diff);
				frames = 0;
				start = stamp;
			}
		}
	}
	frames++;
#endif

	InternalWindow** windows = get_windows();

	Pixel* swapbuffer = get_swapbuffer();
	Pixel* framebuffer = get_framebuffer();

	theme.render_desktop_background();

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i] && windows[i] != get_focused()) {
			render_window_to_swapbuffer(windows[i]);
		}
	}

	if (get_focused() != 0) {
		render_window_to_swapbuffer(get_focused());
	}

	theme.render_cursor();
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		framebuffer[idx] = swapbuffer[idx];
	}

}