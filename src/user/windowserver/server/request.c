#include "request.h"
#include "window.h"
#include "windowserver/config.h"
#include <windowserver/wsmsg.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

void create_window_handler(unsigned sender, CreateWindowRequest* createwindow) {
	InternalWindow* windows = get_windows();
	
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
			win->x = createwindow->x;
			win->y = createwindow->y;
			win->background = createwindow->background;
			res.handle = win->handle;
			render_window(win);
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
	window->present = 0;
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
				if (!labelElements[i].present) {
					window->elements[i] = (InternalElement*) &labelElements[i];
					window->elements[i]->present = 1;
					window->elements[i]->type = WSLabelElement;
					memcpy(labelElements[i].content, labelreq->content, 256);
					labelElements[i].color = labelreq->color;
					labelElements[i].x = labelreq->x;
					labelElements[i].y = labelreq->y;
					labelElements[i].scale = labelreq->scale;
					render_window(window);

					return (WindowServerElementUpdateResponse) { i };
				}
			}
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
				if (!buttonElements[i].present) {
					window->elements[i] = (InternalElement*) &buttonElements[i];
					window->elements[i]->present = 1;
					window->elements[i]->type = WSButtonElement;
					buttonElements[i].background = buttonreq->background;
					buttonElements[i].x = buttonreq->x;
					buttonElements[i].y = buttonreq->y;
					buttonElements[i].width = buttonreq->width;
					buttonElements[i].height = buttonreq->height;
					render_window(window);

					return (WindowServerElementUpdateResponse) { i };
				}
			}
		}
	}

	debugf("No capacity for label\n");
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
			window->events[i].present = 0;

			res.last_event.present = 1;
			res.last_event.element = window->events[i].element;
			res.last_event.type = window->events[i].type;
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