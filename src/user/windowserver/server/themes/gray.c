#include "theme.h"
#include "../window.h"
#include "../input.h"
#include "../buffer.h"
#include "windowserver/config.h"

#include <windowserver/image.h>
#include <windowserver/fontchars.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DESKTOP_BACKGROUND pixel_from_rgb(0xff, 0xff, 0xff)

void themegray_render_text(InternalWindow* window, const char* text, int x, int y, float scale, Pixel* color) {
	// int x0 = x;
	int height = 0;
	for (size_t i = 0; i < strlen(text); i++) {
		FontChar chr = fontchar_for_char(text[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, color, window->raster, chr.width);
		x += (chr.width * scale) + 1;
		height = max(height, chr.height * scale);
	}

	// antialias(window->raster, x - x0, height, x0, y, SUPPORTED_WIDTH, SUPPORTED_HEIGHT);
}

void themegray_render_label(InternalWindow* window, InternalLabelElement* label) {
	themegray_render_text(window, label->content, label->x, window_title_bar_height(window) + label->y, label->scale, &label->color);
}

void themegray_render_button(InternalWindow* window, InternalButtonElement* button) {
	int x0 = button->x;
	int x1 = x0 + button->width;

	int y0 = window_title_bar_height(window) + button->y;
	int y1 = y0 + button->height;
	float frac = (float) button->background.a / (float) 0xff;
	
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			Pixel* out = &window->raster[idx_for_xy(x, y)];
			if (button->background.a == 0xff) {
				*out = button->background;
			} else if (button->background.a != 0) {
				out->r = mix(out->r, button->background.r, frac);
				out->g = mix(out->g, button->background.g, frac);
				out->b = mix(out->b, button->background.b, frac);
				out->a = 0xff;
			}
		}
	}
}

void themegray_render_rectangle(InternalWindow* window, InternalRectangleElement* button) {
	int x0 = button->x;
	int x1 = x0 + button->width;

	int y0 = window_title_bar_height(window) + button->y;
	int y1 = y0 + button->height;
	float frac = (float) button->background.a / (float) 0xff;
	
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			Pixel* out = &window->raster[idx_for_xy(x, y)];
			if (button->background.a == 0xff) {
				*out = button->background;
			} else if (button->background.a != 0) {
				out->r = mix(out->r, button->background.r, frac);
				out->g = mix(out->g, button->background.g, frac);
				out->b = mix(out->b, button->background.b, frac);
				out->a = 0xff;
			}
		}
	}
}

void themegray_render_image(InternalWindow* window, InternalImageElement* image) {
	Bitmap* bitmap = window->bitmaps[image->image_id];

	int xs = bitmap->width;
	int ys = bitmap->height;

	float invscale = 1.0;

	for (int x = 0; x < xs; x++) {
		for (int y = 0; y < ys; y++) {
			if (x + image->x < 0 || x + image->x >= SUPPORTED_WIDTH || y + image->y < 0 || y + image->y >= SUPPORTED_HEIGHT) continue;
			int framebuffer_idx = idx_for_xy(x + image->x, y + image->y);

			int imx = x * invscale;
			int imy = y * invscale;

			int image_idx = idx_for_xyw(imx, imy, bitmap->width);

			Pixel pixel = ((Pixel*) bitmap->data)[image_idx];

			if (pixel.a == 0xff) {
				window->raster[framebuffer_idx] = pixel;
			} else if (pixel.a != 0) {
				float frac = (float) pixel.a / (float) 0xff;
				window->raster[framebuffer_idx].r = mix(window->raster[framebuffer_idx].r, pixel.r, frac);
				window->raster[framebuffer_idx].g = mix(window->raster[framebuffer_idx].g, pixel.g, frac);
				window->raster[framebuffer_idx].b = mix(window->raster[framebuffer_idx].b, pixel.b, frac);
			}
			window->raster[framebuffer_idx].a = 0xff;
		}
	}
}

void themegray_render_window(InternalWindow* window) {
	for (int x = 0; x < (int) window->width; x++) {
		for (int y = 0; y < (int) window->height; y++) {
			int idx = idx_for_xy(x, y);
			if (y < window_title_bar_height(window)) {
				window->raster[idx] = COLOR_LIGHTGREY;
			} else {
				window->raster[idx] = window->background;
			}
		}
	}

	if (window->has_title_bar) {
		for (int x = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; x < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; x++) {
			for (int y = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; y < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; y++) {
				window->raster[idx_for_xy(x, y)] = COLOR_DARKRED;
			}
		}

		FontChar chr = fontchar_for_char('A');
		double paddingpc = 0.5;
		double scale = (TITLE_BAR_HEIGHT * (1.0 - paddingpc)) / chr.height;
		int padding = TITLE_BAR_HEIGHT * paddingpc * 0.5;

		int x = ((TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2) + padding;
		int y = padding;
		themegray_render_text(window, window->title, x, y, scale, &COLOR_VERYDARKGREY);
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0 && window->elements[i]->present) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					themegray_render_label(window, (InternalLabelElement*) window->elements[i]);
					break;
				}
				case WSButtonElement: {
					themegray_render_button(window, (InternalButtonElement*) window->elements[i]);
					break;
				}
				case WSRectangle: {
					themegray_render_rectangle(window, (InternalRectangleElement*) window->elements[i]);
					break;
				}
				case WSImageElement: {
					themegray_render_image(window, (InternalImageElement*) window->elements[i]);
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

void themegray_render_desktop_background() {
	// Pixel* swapbuffer = get_swapbuffer();
	
	// for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
	// 	swapbuffer[idx] = DESKTOP_BACKGROUND;
	// }
}

void themegray_render_cursor_to_swapbuffer() {
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

Theme create_gray_theme() {
	return (Theme) {
		.render_desktop_background = themegray_render_desktop_background,
		.render_window = themegray_render_window,
		.render_cursor = themegray_render_cursor_to_swapbuffer,
	};
}