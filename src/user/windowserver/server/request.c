#include "request.h"
#include "render.h"
#include "window.h"
#include "windowserver/client.h"
#include "windowserver/config.h"
#include "windowserver/image.h"
#include <stdlib.h>
#include <windowserver/wsmsg.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

void find_xy(int* x, int* y) {
	InternalWindow** windows = get_windows();

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i] && windows[i]->x == *x && windows[i]->y == *y) {
			*x += 15;
			*y += 15;

			find_xy(x, y);
		}
	}
}

void create_window_handler(unsigned sender, CreateWindowRequest* createwindow) {
	InternalWindow** windows = get_windows();

	int x = createwindow->x;
	int y = createwindow->y;
	if (createwindow->has_title_bar) find_xy(&x, &y);
	
	CreateWindowResponse res;
	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i] == 0) {
			InternalWindow* win = malloc(sizeof(InternalWindow));
			windows[i] = win;

			memset(win, 0, sizeof(InternalWindow));

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
			set_focused(win);
			break;
		}
	}

	send_ipc_message(sender, &res, sizeof(res));
}

void destroy_window_handler(unsigned sender, DestroyWindowRequest* req) {
	InternalWindow** windows = get_windows();

	if (req->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return;
	}
	InternalWindow* window = windows[req->window];
	if (!window) {
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
	InternalWindow** windows = get_windows();

	if (labelreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[labelreq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			InternalLabelElement* label = malloc(sizeof(InternalLabelElement));
			memset(label, 0, sizeof(InternalLabelElement));

			window->elements[i] = (InternalElement*) label;
			window->elements[i]->present = 1;
			window->elements[i]->type = WSLabelElement;
			window->elements[i]->elementID = i;
			memcpy(label->content, labelreq->content, 256);
			label->color = labelreq->color;
			label->x = labelreq->x;
			label->y = labelreq->y;
			label->scale = labelreq->scale;

			return (WindowServerElementUpdateResponse) { i };
		}
	}

	debugf("No capacity for label\n");
	return (WindowServerElementUpdateResponse) { -1 };
}

WindowServerElementUpdateResponse update_element_label_handler(unsigned sender, WindowServerLabelUpdateRequest* labelreq) {
	InternalWindow** windows = get_windows();

	if (labelreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[labelreq->window];
	if (!window) {
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

	if (!element) {
		debugf("Invalid element ID (not present)\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	memcpy(element->content, labelreq->content, 256);
	element->color = labelreq->color;
	element->x = labelreq->x;
	element->y = labelreq->y;
	element->scale = labelreq->scale;
	
	return (WindowServerElementUpdateResponse) { element->elementID };
}

WindowServerElementUpdateResponse create_element_button_handler(unsigned sender, WindowServerButtonUpdateRequest* buttonreq) {
	InternalWindow** windows = get_windows();

	if (buttonreq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[buttonreq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			InternalButtonElement* button = malloc(sizeof(InternalButtonElement));
			memset(button, 0, sizeof(InternalButtonElement));

			window->elements[i] = (InternalElement*) button;
			window->elements[i]->present = 1;
			window->elements[i]->type = WSButtonElement;
			window->elements[i]->elementID = i;
			button->background = buttonreq->background;
			button->x = buttonreq->x;
			button->y = buttonreq->y;
			button->width = buttonreq->width;
			button->height = buttonreq->height;

			return (WindowServerElementUpdateResponse) { i };
		}
	}

	debugf("No capacity for button\n");
	return (WindowServerElementUpdateResponse) { -1 };
}

WindowServerElementUpdateResponse update_element_rectangle_handler(unsigned sender, WindowServerRectangleUpdateRequest* rectanglereq) {
	InternalWindow** windows = get_windows();

	if (rectanglereq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[rectanglereq->window];
	if (!window) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (rectanglereq->elementId >= WINDOW_ELEMENTS_CAPACITY) {
		debugf("Invalid element ID (does not exist)\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	
	InternalRectangleElement* element = (InternalRectangleElement*) window->elements[rectanglereq->elementId];
	if (element->type != WSRectangle) {
		debugf("Not a rectangle\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!element) {
		debugf("Invalid element ID (not present)\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	element->x = rectanglereq->x;
	element->y = rectanglereq->y;
	element->width = rectanglereq->width;
	element->height = rectanglereq->height;
	element->background = rectanglereq->background;
	
	return (WindowServerElementUpdateResponse) { element->elementID };
}

WindowServerElementUpdateResponse create_element_rectangle_handler(unsigned sender, WindowServerRectangleUpdateRequest* rectanglereq) {
	InternalWindow** windows = get_windows();

	if (rectanglereq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[rectanglereq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			InternalRectangleElement* rectangle = malloc(sizeof(InternalRectangleElement));
			memset(rectangle, 0, sizeof(InternalRectangleElement));

			window->elements[i] = (InternalElement*) rectangle;
			window->elements[i]->present = 1;
			window->elements[i]->type = WSRectangle;
			window->elements[i]->elementID = i;
			rectangle->background = rectanglereq->background;
			rectangle->x = rectanglereq->x;
			rectangle->y = rectanglereq->y;
			rectangle->width = rectanglereq->width;
			rectangle->height = rectanglereq->height;

			return (WindowServerElementUpdateResponse) { i };
		}
	}

	debugf("No capacity for rectangle\n");
	return (WindowServerElementUpdateResponse) { -1 };
}

WindowServerElementUpdateResponse create_element_image_handler(unsigned sender, WindowServerImageUpdateRequest* imagereq) {
	InternalWindow** windows = get_windows();

	if (imagereq->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}
	InternalWindow* window = windows[imagereq->window];
	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (!window) {
		debugf("Window does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	if (imagereq->image_id < 0 || imagereq->image_id >= WINDOW_BITMAPS_CAPACITY || window->bitmaps[imagereq->image_id] == 0) {
		debugf("Bitmap does not exist\n");
		return (WindowServerElementUpdateResponse) { -1 };
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] == 0) {
			InternalImageElement* image = malloc(sizeof(InternalImageElement));
			memset(image, 0, sizeof(InternalImageElement));

			window->elements[i] = (InternalElement*) image;
			window->elements[i]->present = 1;
			window->elements[i]->type = WSImageElement;
			window->elements[i]->elementID = i;
			image->image_id = imagereq->image_id;
			image->x = imagereq->x;
			image->y = imagereq->y;
			image->width = imagereq->width;
			image->height = imagereq->height;

			return (WindowServerElementUpdateResponse) { i };
		}
	}

	debugf("No capacity for image element\n");
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
			case WSRectangle: {
				return create_element_rectangle_handler(sender, (WindowServerRectangleUpdateRequest*) req);
			}
			case WSImageElement: {
				return create_element_image_handler(sender, (WindowServerImageUpdateRequest*) req);
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
			case WSRectangle: {
				return update_element_rectangle_handler(sender, (WindowServerRectangleUpdateRequest*) req);
			}
			default: {
				debugf("Unknown element type 0x%.8x\n", req->elementType);
				return (WindowServerElementUpdateResponse) { -1 };
			}
		}
	}
}

void window_status_handler(unsigned sender, WindowStatusRequest* req) {
	InternalWindow** windows = get_windows();
	WindowStatusResponse res;
	memset(&res, 0, sizeof(res));
			
	if (req->window >= WINDOWS_CAPACITY) {
		res.present = 0;
		send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
		return;
	}
	InternalWindow* window = windows[req->window];

	if (!window) {
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

void load_image_handler(unsigned sender, ImageLoadRequest* req) {
	InternalWindow** windows = get_windows();
	ImageLoadResponse res;
	memset(&res, 0, sizeof(res));
			
	if (req->window >= WINDOWS_CAPACITY) {
		res.success = 0;
		send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
		return;
	}
	InternalWindow* window = windows[req->window];

	if (!window) {
		res.success = 0;
		goto send;
	}

	if (window->creatorpid != sender) {
		res.success = 0;
		goto send;
	}

	Bitmap* bitmap = load_bmp(req->path);
	if (bitmap == 0 || bitmap->data == 0) {
		res.success = 0;
		goto send;
	}
	res.success = 1;

	for (int i = 0; i < WINDOW_BITMAPS_CAPACITY; i++) {
		if (window->bitmaps[i] == 0) {
			window->bitmaps[i] = bitmap;
			res.id = i;
			break;
		}
	}

	res.width = bitmap->width;
	res.height = bitmap->height;

	send:
	send_ipc_message(sender, &res, sizeof(ImageLoadResponse));
}

void render_window_handler(unsigned sender, WindowRenderRequest* req) {
	InternalWindow** windows = get_windows();

	if (req->window >= WINDOWS_CAPACITY) {
		debugf("Invalid window ID\n");
		return;
	}
	InternalWindow* window = windows[req->window];
	if (!window) {
		debugf("Window does not exist\n");
		return;
	}

	if (window->creatorpid != sender) {
		debugf("Invalid permissions for window\n");
		return;
	}

	get_theme()->render_window(window);
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
		case WSLoadImage: {
			load_image_handler(sender, (ImageLoadRequest*) raw);
			return;
		}
		case WSRenderWindow: {
			render_window_handler(sender, (WindowRenderRequest*) raw);
			return;
		}
		default: {
			debugf("Unknown command %d\n", action);
			return;
		}
	}
}