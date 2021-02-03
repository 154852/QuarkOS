#include "input.h"
#include <ckeyboard.h>
#include "window.h"
#include "windowserver/client.h"
#include <syscall.h>
#include <cmouse.h>
#include <assertions.h>

static unsigned mouse_socket;
static int mouse_x = SUPPORTED_WIDTH / 2;
static int mouse_y = SUPPORTED_HEIGHT / 2;
static char is_clicking = 0;

void initialise_mouse() {
	mouse_socket = open("/dev/mouse", FILE_FLAG_R | FILE_FLAG_SOCK);
}

static unsigned keyboard_socket;
static char keydata[sizeof(KeyEvent)];

InternalWindow* dragging;

void initialise_keyboard() {
	keyboard_socket = open("/dev/keyboard", FILE_FLAG_R | FILE_FLAG_SOCK);

	assert(sizeof(WindowServerKeyboardEvent) == sizeof(WindowServerEvent));
}

void update_keyboard() {
	unsigned len = read(keyboard_socket, keydata, sizeof(KeyEvent));
	while (len != 0) {
		KeyEvent* state = (KeyEvent*) keydata;

		if (get_focused() != 0) {
			WindowServerKeyboardEvent* event = (WindowServerKeyboardEvent*) allocate_event(get_focused());
			assert(event);
			event->element = 0;
			event->type = WSEvKeyPress;
			event->event = *state;
		}

		len = read(keyboard_socket, keydata, sizeof(KeyEvent));
	}
}

char window_contains(InternalWindow* window, int x, int y) {
	return rect_contains(window->x, window->y, (int) window->width, (int) window->height, x, y);
}

void window_resolve_click(InternalWindow* window, int x, int y) {
	set_focused(window);

	// X button
	if (window->has_title_bar && rect_contains((TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, x, y)) {
		destroy_internal_window(window);
		return;
	}

	if (window->has_title_bar && y < TITLE_BAR_HEIGHT) {
		dragging = window;
		return;
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] && window->elements[i]->type == WSButtonElement) {
			InternalButtonElement* button = (InternalButtonElement*) window->elements[i];
			if (rect_contains(button->x, button->y + window_title_bar_height(window), (int) button->width, (int) button->height, x, y)) {
				WindowServerEvent* event = allocate_event(window);
				assert(event);
				event->element = button->elementID;
				event->type = WSEvButtonClick;
				return;
			}
		}
	}
}

void resolve_click(int x, int y) {
	InternalWindow* focused = get_focused();
	if (focused != 0 && window_contains(focused, x, y)) {
		window_resolve_click(focused, x - focused->x, y - focused->y);
		return;
	}

	InternalWindow** windows = get_windows();
	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i] && windows[i] != focused && window_contains(windows[i], x, y)) {
			window_resolve_click(windows[i], x - windows[i]->x, y - windows[i]->y);
			return;
		}
	}
}

void update_cursor() {
	MousePacket packet;
	unsigned length = read(mouse_socket, &packet, sizeof(MousePacket));
	
	while (length != 0) {
		assert(length == sizeof(MousePacket));
		mouse_x = clamp(mouse_x + packet.x_delta, 0, SUPPORTED_WIDTH);
		mouse_y = clamp(mouse_y - packet.y_delta, 0, SUPPORTED_HEIGHT);

		if (packet.flags.left_button != is_clicking) {
			if (packet.flags.left_button) {
				resolve_click(mouse_x, mouse_y);
			}
			is_clicking = packet.flags.left_button;
		}

		if (packet.flags.left_button && dragging != 0) {
			dragging->x += packet.x_delta;
			dragging->y -= packet.y_delta;
		} else if (!packet.flags.left_button && dragging != 0) {
			dragging = 0;
		}

		length = read(mouse_socket, &packet, sizeof(MousePacket));
	}
}

int get_mouse_x() {
	return mouse_x;
}

int get_mouse_y() {
	return mouse_y;
}