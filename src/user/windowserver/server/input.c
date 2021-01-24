#include "input.h"
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

char window_contains(InternalWindow* window, int x, int y) {
	return rect_contains(window->x, window->y, (int) window->width, (int) window->height, x, y);
}

void window_resolve_click(InternalWindow* window, int x, int y) {
	// X button
	if (rect_contains((TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, x, y)) {
		window->present = 0;
		return;
	}
}

void resolve_click(int x, int y) {
	InternalWindow* windows = get_windows();

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present && window_contains(&windows[i], x, y)) {
			window_resolve_click(&windows[i], x - windows[i].x, y - windows[i].y);
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

		length = read(mouse_socket, &packet, sizeof(MousePacket));
	}
}

int get_mouse_x() {
	return mouse_x;
}

int get_mouse_y() {
	return mouse_y;
}