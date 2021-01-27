#include "theme.h"
#include "../window.h"
#include "../input.h"
#include "../buffer.h"
#include "windowserver/config.h"
#include "windowserver/themes/color-active.h"

#include <windowserver/image.h>
#include <windowserver/fontchars.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void themehacker_render_text(InternalWindow* window, const char* text, int x, int y, float scale, Pixel* color) {
	int x0 = x;
	int height = 0;
	for (size_t i = 0; i < strlen(text); i++) {
		FontChar chr = fontchar_for_char(text[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, color, window->raster, chr.width);
		x += (chr.width * scale) + 1;
		height = max(height, chr.height * scale);
	}

	antialias(window->raster, x - x0, height, x0, y, SUPPORTED_WIDTH, SUPPORTED_HEIGHT);
}

void themehacker_render_label(InternalWindow* window, InternalLabelElement* label) {
	themehacker_render_text(window, label->content, label->x, window_title_bar_height(window) + label->y, label->scale, &label->color);
}

void themehacker_render_button(InternalWindow* window, InternalButtonElement* button) {
	int x0 = button->x;
	int x1 = x0 + button->width;

	int y0 = window_title_bar_height(window) + button->y;
	int y1 = y0 + button->height;
	
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			window->raster[idx_for_xy(x, y)] = button->background;
		}
	}
}

void themehacker_render_window(InternalWindow* window) {
	for (int x = 0; x < (int) window->width; x++) {
		for (int y = 0; y < (int) window->height; y++) {
			if (x + window->x < 0) continue;
			if (y + window->y < 0) continue;
			if (x + window->x >= SUPPORTED_WIDTH) continue;
			if (y + window->y >= SUPPORTED_HEIGHT) continue;

			int idx = idx_for_xy(x, y);
			if (y < window_title_bar_height(window)) {
				window->raster[idx] = COLOR_SECONDARY_BACKGROUND;
			} else {
				window->raster[idx] = window->background;
			}
		}
	}

	if (window->has_title_bar) {
		int start = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2;
		int end = (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2;

		for (int x = start; x < end; x++) {
			for (int y = start; y < end; y++) {
				if (!(x == start || x == end - 1 || y == start || y == end - 1)) continue;
				window->raster[idx_for_xy(x, y)] = COLOR_DARKRED;
			}
		}

		FontChar chr = fontchar_for_char('A');
		double paddingpc = 0.5;
		double scale = (TITLE_BAR_HEIGHT * (1.0 - paddingpc)) / chr.height;
		int padding = TITLE_BAR_HEIGHT * paddingpc * 0.5;

		int x = ((TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2) + padding;
		int y = padding;
		themehacker_render_text(window, window->title, x, y, scale, &DEFAULT_TEXT_COLOR);
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0 && window->elements[i]->present) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					themehacker_render_label(window, (InternalLabelElement*) window->elements[i]);
					break;
				}
				case WSButtonElement: {
					themehacker_render_button(window, (InternalButtonElement*) window->elements[i]);
					break;
				}
				default: {
					debugf("Unknown element type in render %d\n", window->elements[i]->type);
					break;
				}
			}
		}
	}
}

void themehacker_render_desktop_background() {
	Pixel* swapbuffer = get_swapbuffer();
	
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		swapbuffer[idx] = pixel_from_rgb(0x10, 0x10, 0x10);
	}
}

#define CURSOR_SIZE 4
void themehacker_render_cursor_to_swapbuffer() {
	Pixel* swapbuffer = get_swapbuffer();

	int x0 = get_mouse_x();
	int y0 = get_mouse_y();
	
	for (int x = -CURSOR_SIZE; x <= CURSOR_SIZE; x++) {
		if (x0 + x < 0 || x0 + x >= SUPPORTED_WIDTH) continue;
		if (y0 < 0 || y0 >= SUPPORTED_HEIGHT) continue;

		int idx = idx_for_xy(x0 + x, y0);
		swapbuffer[idx] = DEFAULT_TEXT_COLOR;
	}

	for (int y = -CURSOR_SIZE; y <= CURSOR_SIZE; y++) {
		if (y0 + y < 0 || y0 + y >= SUPPORTED_HEIGHT) continue;
		if (x0 < 0 || x0 >= SUPPORTED_WIDTH) continue;

		int idx = idx_for_xy(x0, y0 + y);
		swapbuffer[idx] = DEFAULT_TEXT_COLOR;
	}
}

Theme create_hacker_theme() {
	return (Theme) {
		.render_desktop_background = themehacker_render_desktop_background,
		.render_window = themehacker_render_window,
		.render_cursor = themehacker_render_cursor_to_swapbuffer,
	};
}