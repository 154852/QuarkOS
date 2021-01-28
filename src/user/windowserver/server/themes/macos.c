#include "theme.h"
#include "../window.h"
#include "../input.h"
#include "../buffer.h"

#include <windowserver/image.h>
#include <windowserver/fontchars.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DESKTOP_BACKGROUND pixel_from_rgb(255, 230, 244)

void thememacos_render_text(InternalWindow* window, const char* text, int x, int y, float scale, Pixel* color) {
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

void thememacos_render_label(InternalWindow* window, InternalLabelElement* label) {
	thememacos_render_text(window, label->content, label->x, window_title_bar_height(window) + label->y, label->scale, &label->color);
}

void thememacos_render_button(InternalWindow* window, InternalButtonElement* button) {
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

void thememacos_render_window(InternalWindow* window) {
	for (int x = 0; x < (int) window->width; x++) {
		for (int y = 0; y < (int) window->height; y++) {
			int idx = idx_for_xy(x, y);
			if (x == 0 || x == (int) window->width - 1 || y == (int) window->height - 1 || y == window_title_bar_height(window) || y == 0) {
				window->raster[idx] = pixel_from_rgb(212, 212, 212);
			} else if (y < window_title_bar_height(window)) {
				window->raster[idx] = pixel_from_rgb(249, 240, 241);
			} else {
				window->raster[idx] = window->background;
			}
		}
	}

	if (window->has_title_bar) {
		int radius = (WINDOW_BUTTON_SIZE / 2) - 2;

		for (int x = 0; x < TITLE_BAR_HEIGHT; x++) {
			for (int y = 0; y < TITLE_BAR_HEIGHT; y++) {
				int xo = x - TITLE_BAR_HEIGHT/2;
				int yo = y - TITLE_BAR_HEIGHT/2;

				if ((xo * xo) + (yo * yo) < (radius * radius)) {
					window->raster[idx_for_xy(x, y)] = COLOR_DARKRED;
				} else if ((xo * xo) + (yo * yo) < ((radius + 1) * (radius + 1))) {
					window->raster[idx_for_xy(x, y)] = pixel_from_rgb(mix(0xd0, 249, 0.5), mix(0, 240, 0.5), mix(0, 241, 0.5));
				}
			}
		}

		FontChar chr = fontchar_for_char('A');
		double paddingpc = 0.5;
		double scale = (TITLE_BAR_HEIGHT * (1.0 - paddingpc)) / chr.height;
		int padding = TITLE_BAR_HEIGHT * paddingpc * 0.5;

		int y = padding;

		int width = 0;
		for (size_t i = 0; i < strlen(window->title); i++) {
			FontChar chr = fontchar_for_char(window->title[i]);
			width += (chr.width * scale) + 1;
		}

		int x = ((window->width - width) / 2);

		thememacos_render_text(window, window->title, x, y, scale, &COLOR_VERYDARKGREY);
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0 && window->elements[i]->present) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					thememacos_render_label(window, (InternalLabelElement*) window->elements[i]);
					break;
				}
				case WSButtonElement: {
					thememacos_render_button(window, (InternalButtonElement*) window->elements[i]);
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

void thememacos_render_desktop_background() {
	Pixel* swapbuffer = get_swapbuffer();
	
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		swapbuffer[idx] = DESKTOP_BACKGROUND;
	}
}

void thememacos_render_cursor_to_swapbuffer() {
	int x0 = clamp(get_mouse_x(), 1, SUPPORTED_WIDTH - 2);
	int y0 = clamp(get_mouse_y(), 1, SUPPORTED_HEIGHT - 2);

	Pixel* swapbuffer = get_swapbuffer();
	
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			int idx = idx_for_xy(x0 + x, y0 + y);
			swapbuffer[idx] = (x == 0 && y == 0)? COLOR_LIGHTGREY:COLOR_BLACK;
		}
	}
}

Theme create_macos_theme() {
	return (Theme) {
		.render_desktop_background = thememacos_render_desktop_background,
		.render_window = thememacos_render_window,
		.render_cursor = thememacos_render_cursor_to_swapbuffer,
	};
}