#include "request.h"
#include "window.h"
#include "windowserver/client.h"
#include "windowserver/config.h"
#include <windowserver/wsmsg.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

void find_xy(int* x, int* y) {
	InternalWindow* windows = get_windows();

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present && windows[i].x == *x && windows[i].y == *y) {
			*x += 15;
			*y += 15;

			find_xy(x, y);
		}
	}
}

void create_window_handler(unsigned sender, CreateWindowRequest* createwindow) {
	InternalWindow* windows = get_windows();

	int x = createwindow->x;
	int y = createwindow->y;
	find_xy(&x, &y);
	
	CreateWindowResponse res;
	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (!windows[i].present) {
			InternalWindow* win = &windows[i];
			memset(win, 0, sizeof(InternalWindow));

			win->present = 1;
			win->handle = i;
			win->creatorpid = sender;

			memcpy(win->title, createwindow->title, 64);
			win->width = createwindow->width;
			win->height = createwindow->height;
			win->x = x;
			win->y = y;
			win->background = createwindow->background;
			win->has_title_bar = createwindow->has_title_bar;
			res.handle = win->handle;
			render_window(win);
			set_focused(win);
			break;
		}
	}

	send_ipc_message(sender, &res, sizeof(res));
}

void destroy_window_handler(unsigned sender, DestroyWindowRequest* req) {
	InternalWindow* windows = get_windows();

	if (req->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return;
	}
	InternalWindow* window = &windows[req->window];
	if (!window->present) {
		debugf("Window does not exist\n");
		return;
	}

	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return;
	}

	destroy_internal_window(window);
}

WindowServerElementUpdateResponse create_element_label_handler(unsigned sender, WindowServerLabelUpdateRequest* labelreq) {
	InternalWindow* windows = get_windows();
	InternalLabelElement* labelElements = get_label_elements();

	if (labelreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = &windows[labelreq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window->present) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			for (int j = 0; j < LABELS_CAPACITY; j++) {
				if (!labelElements[j].present) {
					window->elements[i] = (InternalElement*) &labelElements[j];
					window->elements[i]->present = 1;
					window->elements[i]->type = WSLabelElement;
					window->elements[i]->elementID = i;
					memcpy(labelElements[j].content, labelreq->content, 256);
					labelElements[j].color = labelreq->color;
					labelElements[j].x = labelreq->x;
					labelElements[j].y = labelreq->y;
					labelElements[j].scale = labelreq->scale;
					render_window(window);

					return (WindowServerElementUpdateResponse) { i };
				}
			}

			debugf("No capacity for label\n");
			return (WindowServerElementUpdateResponse) { -1 };
		}
	}

	debugf("No capacity for label\n");
	return (WindowServerElementUpdateResponse) { -1 };
}

WindowServerElementUpdateResponse update_element_label_handler(unsigned sender, WindowServerLabelUpdateRequest* labelreq) {
	InternalWindow* windows = get_windows();

	if (labelreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = &windows[labelreq->window];
	if (!window->present) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (labelreq->elementId >= WINDOW_ELEMENTS_CAPACITY) {
		debugf("Invalid element ID (does not exist)\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	
	InternalLabelElement* element = (InternalLabelElement*) window->elements[labelreq->elementId];
	if (element->type != WSLabelElement) {
		debugf("Not a label\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!element->present) {
		debugf("Invalid element ID (not present)\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	memcpy(element->content, labelreq->content, 256);
	element->color = labelreq->color;
	element->x = labelreq->x;
	element->y = labelreq->y;
	element->scale = labelreq->scale;
	render_window(window);
	
	return (WindowServerElementUpdateResponse) { element->elementID };
}

WindowServerElementUpdateResponse create_element_button_handler(unsigned sender, WindowServerButtonUpdateRequest* buttonreq) {
	InternalWindow* windows = get_windows();
	InternalButtonElement* buttonElements = get_button_elements();

	if (buttonreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = &windows[buttonreq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window->present) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			for (int j = 0; j < BUTTONS_CAPACITY; j++) {
				if (!buttonElements[j].present) {
					window->elements[i] = (InternalElement*) &buttonElements[j];
					window->elements[i]->present = 1;
					window->elements[i]->type = WSButtonElement;
					window->elements[i]->elementID = i;
					buttonElements[j].background = buttonreq->background;
					buttonElements[j].x = buttonreq->x;
					buttonElements[j].y = buttonreq->y;
					buttonElements[j].width = buttonreq->width;
					buttonElements[j].height = buttonreq->height;
					render_window(window);

					return (WindowServerElementUpdateResponse) { i };
				}
			}
		}
	}

	debugf("No capacity for button\n");
	return (WindowServerElementUpdateResponse) { -1 };
}

WindowServerElementUpdateResponse update_element_handler(unsigned sender, WindowServerElementUpdateRequest* req) {
	if ((int) req->elementId == -1) {
		switch (req->elementType) {
			case WSLabelElement: {
				return create_element_label_handler(sender, (WindowServerLabelUpdateRequest*) req);
			}
			case WSButtonElement: {
				return create_element_button_handler(sender, (WindowServerButtonUpdateRequest*) req);
			}
			default: {
				debugf("Unknown element type 0x%.8x\n", req->elementType);
				return (WindowServerElementUpdateResponse) { -1 };
			}
		}
	} else {
		switch (req->elementType) {
			case WSLabelElement: {
				return update_element_label_handler(sender, (WindowServerLabelUpdateRequest*) req);
			}
			default: {
				debugf("Unknown element type 0x%.8x\n", req->elementType);
				return (WindowServerElementUpdateResponse) { -1 };
			}
		}
	}
}

void window_status_handler(unsigned sender, WindowStatusRequest* req) {
	InternalWindow* windows = get_windows();
	WindowStatusResponse res;
	memset(&res, 0, sizeof(res));
			
	if (req->window >= WINDOWS_CAPACITY) {
		res.present = 0;
		send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
		return;
	}
	InternalWindow* window = &windows[req->window];

	if (!window->present) {
		res.present = 0;
		send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
		return;
	}
	res.present = 1;

	if (window->creatorpid != sender) {
		send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
		return;
	}
	
	res.present = 1;
	res.x = window->x;
	res.y = window->y;
	res.width = window->width;
	res.height = window->height;

	res.last_event.present = 0;
	for (int i = WINDOW_EVENTS_CAPACITY - 1; i >= 0; i--) {
		if (window->events[i].present) {
			res.last_event = window->events[i];
			memset(&window->events[i], 0, sizeof(WindowServerEvent));
			break;
		}
	}

	send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
}

void handle_request(unsigned action, unsigned sender, void* raw) {
	switch (action) {
		case WSCreateWindow: {
			create_window_handler(sender, (CreateWindowRequest*) raw);
			return;
		}
		case WSDestroyWindow: {
			destroy_window_handler(sender, (DestroyWindowRequest*) raw);
			return;
		}
		case WSUpdateElement: {
			WindowServerElementUpdateResponse res = update_element_handler(sender, (WindowServerElementUpdateRequest*) raw);
			send_ipc_message(sender, &res, sizeof(WindowServerElementUpdateResponse));
			return;
		}
		case WSWindowStatus: {
			window_status_handler(sender, (WindowStatusRequest*) raw);
			return;
		}
		default: {
			debugf("Unknown command %d\n", action);
			return;
		}
	}
}