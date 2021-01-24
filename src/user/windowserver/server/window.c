#include "window.h"
#include "windowserver/config.h"
#include <windowserver/image.h>
#include <windowserver/wsmsg.h>
#include <windowserver/fontchars.h>
#include <stdio.h>
#include <string.h>

static InternalWindow windows[WINDOWS_CAPACITY];
static InternalLabelElement labelElements[LABELS_CAPACITY];
static InternalButtonElement buttonElements[BUTTONS_CAPACITY];

InternalWindow* get_windows() {
	return windows;
}

InternalLabelElement* get_label_elements() {
	return labelElements;
}

InternalButtonElement* get_button_elements() {
	return buttonElements;
}

WindowServerEvent* allocate_event(InternalWindow* window) {
	for (int i = 0; i < WINDOW_EVENTS_CAPACITY; i++) {
		if (!window->events[i].present) {
			window->events[i].present = 1;
			return &window->events[i];
		}
	}
	
	return 0;
}

void render_label(InternalWindow* window, InternalLabelElement* label) {
	int x = label->x;
	int y = window_title_bar_height(window) + label->y;
	double scale = label->scale;
	for (size_t i = 0; i < strlen(label->content); i++) {
		FontChar chr = fontchar_for_char(label->content[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &label->color, window->raster, chr.width);
		x += (chr.width * scale) + 1;
	}
}

void render_button(InternalWindow* window, InternalButtonElement* button) {
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

void render_window(InternalWindow* window) {
	for (int x = 0; x < (int) window->width; x++) {
		for (int y = 0; y < (int) window->height; y++) {
			if (x + window->x < 0) continue;
			if (y + window->y < 0) continue;
			if (x + window->x >= SUPPORTED_WIDTH) continue;
			if (y + window->y >= SUPPORTED_HEIGHT) continue;

			int idx = idx_for_xy(x, y);
			if (x == 0 || x == (int) window->width - 1 || y == (int) window->height - 1 || y == window_title_bar_height(window) || y == 0) {
				window->raster[idx] = COLOR_VERYLIGHTGREY;
			} else if (y < window_title_bar_height(window)) {
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
		for (size_t i = 0; i < strlen(window->title); i++) {
			FontChar chr = fontchar_for_char(window->title[i]);
			if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &COLOR_DARKGREY, window->raster, chr.width);
			x += (chr.width * scale) + 1;
		}

	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					render_label(window, (InternalLabelElement*) window->elements[i]);
					break;
				}
				case WSButtonElement: {
					render_button(window, (InternalButtonElement*) window->elements[i]);
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

static InternalWindow* focused_window = 0;

InternalWindow* get_focused() {
	return focused_window;
}

void set_focused(InternalWindow* window) {
	focused_window = window;
}