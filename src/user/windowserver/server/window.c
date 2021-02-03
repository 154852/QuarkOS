#include "window.h"
#include "windowserver/client.h"
#include "windowserver/color.h"
#include "windowserver/config.h"
#include <windowserver/image.h>
#include <windowserver/wsmsg.h>
#include <windowserver/fontchars.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static InternalWindow* windows[WINDOWS_CAPACITY];

InternalWindow** get_windows() {
	return windows;
}

void destroy_internal_window(InternalWindow* window) {
	if (get_focused() == window) set_focused(0);

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					free_sized(window->elements[i], sizeof(InternalLabelElement));
					break;
				}
				case WSButtonElement: {
					free_sized(window->elements[i], sizeof(InternalButtonElement));
					break;
				}
				case WSRectangle: {
					free_sized(window->elements[i], sizeof(InternalRectangleElement));
					break;
				}
				default: {
					debugf("Unknown element type in window destroy %d\n", window->elements[i]->type);
				}
			}
		}
	}

	for (int i = 0; i < WINDOW_BITMAPS_CAPACITY; i++) {
		if (window->bitmaps[i] != 0) {
			free_sized(window->bitmaps[i], sizeof(Bitmap));
		}
	}

	windows[window->handle] = 0;
	free_sized(window, sizeof(InternalWindow));
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

static InternalWindow* focused_window = 0;

InternalWindow* get_focused() {
	return focused_window;
}

void set_focused(InternalWindow* window) {
	focused_window = window;
}