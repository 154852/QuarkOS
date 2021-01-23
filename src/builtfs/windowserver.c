#include "wsmsg.h"
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <string.h>
#include "fontchars.h"
#include <kernel/cmouse.h>

#define SUPPORTED_WIDTH 1024
#define SUPPORTED_HEIGHT 768
#define SUPPORTED_SIZE (SUPPORTED_WIDTH * SUPPORTED_HEIGHT)
#define FULL_SIZE (SUPPORTED_SIZE * sizeof(unsigned))

#define max(a, b) ((a) > (b)? (a):(b))
#define min(a, b) ((a) < (b)? (a):(b))
#define clamp(x, a, b) min(max(x, a), b)

Pixel pixel_from_hex(unsigned hex) {
	if (hex > 0xffffff) {
		return (Pixel) {
			.b = hex & 0xff,
			.g = (hex >> 8) & 0xff,
			.r = (hex >> 16) & 0xff,
			.a = (hex >> 24) & 0xff
		};
	}

	if (hex > 0xfff) {
		return (Pixel) {
			.b = hex & 0xff,
			.g = (hex >> 8) & 0xff,
			.r = (hex >> 16) & 0xff,
			.a = 0xff
		};
	}

	return (Pixel) {
		.b = (hex & 0xf) * 0xf,
		.g = ((hex >> 4) & 0xf) * 0xf,
		.r = ((hex >> 12) & 0xf) * 0xf,
		.a = 0xff
	};
}

#define pixel_from_rgb(_r, _g, _b) { .r = _r, .g = _g, .b = _b, .a = 0xff };

const Pixel COLOR_BLACK = pixel_from_rgb(0, 0, 0);
const Pixel COLOR_DARKGREY = pixel_from_rgb(0x60, 0x60, 0x60);
const Pixel COLOR_LIGHTGREY = pixel_from_rgb(0xe0, 0xe0, 0xe0);
const Pixel COLOR_VERYLIGHTGREY = pixel_from_rgb(0xc0, 0xc0, 0xc0);
const Pixel COLOR_RED = pixel_from_rgb(0xff, 0, 0);
const Pixel COLOR_DARKRED = pixel_from_rgb(0xd0, 0, 0);

Pixel desktopBackground;

Pixel data[SUPPORTED_SIZE];
FrameBufferInfo info;
Pixel* framebuffer;

void blit() {
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		framebuffer[idx] = data[idx];
	}
}

#define idx_for_xy(x, y) (((int) (y) * info.width) + (int) (x))
#define idx_for_xyw(x, y, w) (((int) (y) * (int) (w)) + (int) (x))
#define mix(a, b, frac) ((((float) (b) - (float) (a)) * (float) (frac)) + (float) (a))
#define rect_contains(rx, ry, rw, rh, x, y) ((x) >= (rx) && (x) <= ((rx) + (rw)) && (y) >= (ry) && (y) <= ((ry) + (rh)))

void copy_image(int x0, int y0, Pixel* image, int w, int h, double scale, const Pixel* color, Pixel* out, int memw) {
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int image_idx = idx_for_xyw(x, y, memw);
			int framebuffer_idx = idx_for_xy((x * scale) + x0, (y * scale) + y0);

			Pixel pixel = image[image_idx];
			if (color != 0) pixel = *color;

			if (image[image_idx].a == 0xff) {
				out[framebuffer_idx] = pixel;
			} else if (image[image_idx].a != 0) {
				float frac = (float) image[image_idx].a / (float) 0xff;
				out[framebuffer_idx].r = mix(out[framebuffer_idx].r, pixel.r, frac);
				out[framebuffer_idx].g = mix(out[framebuffer_idx].g, pixel.g, frac);
				out[framebuffer_idx].b = mix(out[framebuffer_idx].b, pixel.b, frac);
			}
			out[framebuffer_idx].a = 0xff;
		}
	}
}

#define TITLE_BAR_HEIGHT 30

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
} InternalElement;

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
	
	char content[256];
	int x;
	int y;
	Pixel color;
	float scale;
} InternalLabelElement;

#define WINDOW_ELEMENTS_CAPACITY 32
typedef struct {
	char present;
	unsigned handle;
	unsigned creatorpid;

	char title[64];
	unsigned width;
	unsigned height;
	int x;
	int y;

	Pixel background;
	Pixel raster[SUPPORTED_SIZE];

	InternalElement* elements[WINDOW_ELEMENTS_CAPACITY];
} InternalWindow;

#define WINDOWS_CAPACITY 32
static InternalWindow windows[WINDOWS_CAPACITY];
#define LABELS_CAPACITY 32
static InternalLabelElement labelElements[LABELS_CAPACITY];

#define WINDOW_BUTTON_SIZE 15

void render_label(InternalWindow* window, InternalLabelElement* label) {
	int x = label->x;
	int y = TITLE_BAR_HEIGHT + label->y;
	double scale = label->scale;
	for (size_t i = 0; i < strlen(label->content); i++) {
		FontChar chr = fontchar_for_char(label->content[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &label->color, window->raster, chr.width);
		x += (chr.width * scale) + 1;
	}
}

void render_window(InternalWindow* window) {
	for (int x = 0; x < (int) window->width; x++) {
		for (int y = 0; y < (int) window->height; y++) {
			if (x + window->x < 0) continue;
			if (y + window->y < 0) continue;
			if (x + window->x >= info.width) continue;
			if (y + window->y >= info.height) continue;

			int idx = idx_for_xy(x, y);
			if (x == 0 || x == (int) window->width - 1 || y == (int) window->height - 1 || y == TITLE_BAR_HEIGHT || y == 0) {
				window->raster[idx] = COLOR_VERYLIGHTGREY;
			} else if (y < TITLE_BAR_HEIGHT) {
				window->raster[idx] = COLOR_LIGHTGREY;
			} else {
				window->raster[idx] = window->background;
			}
		}
	}

	for (int x = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; x < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; x++) {
		for (int y = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; y < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; y++) {
			window->raster[idx_for_xy(x, y)] = COLOR_DARKRED;
		}
	}

	FontChar chr = fontchar_for_char('A');
	double paddingpc = 0.6;
	double scale = (TITLE_BAR_HEIGHT * (1.0 - paddingpc)) / chr.height;
	int padding = TITLE_BAR_HEIGHT * paddingpc * 0.5;

	int x = ((TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2) + padding;
	int y = padding;
	for (size_t i = 0; i < strlen(window->title); i++) {
		FontChar chr = fontchar_for_char(window->title[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &COLOR_DARKGREY, window->raster, chr.width);
		x += (chr.width * scale) + 1;
	}

	for (int i = 0; i < WINDOW_ELEMENTS_CAPACITY; i++) {
		if (window->elements[i] != 0) {
			switch (window->elements[i]->type) {
				case WSLabelElement: {
					render_label(window, (InternalLabelElement*) window->elements[i]);
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

static unsigned mouse_socket;
static int mouse_x;
static int mouse_y;

void render_cursor_to_swapbuffer() {
	int x0 = clamp(mouse_x, 1, info.width - 1);
	int y0 = clamp(mouse_y, 1, info.height - 1);
	
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			int idx = idx_for_xy(x0 + x, y0 + y);
			data[idx] = COLOR_RED;
		}
	}
}

void render_window_to_swapbuffer(InternalWindow* window) {
	for (int y = 0; y < (int) window->height; y++) {
		int outoffset = idx_for_xy(window->x, y + window->y);
		int offset = idx_for_xyw(0, y, SUPPORTED_WIDTH);
		for (int x = 0; x < (int) window->width; x++) {
			data[outoffset + x] = window->raster[offset + x];
		}
	}
}

void render() {
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		data[idx] = desktopBackground;
	}

	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present) {
			render_window_to_swapbuffer(&windows[i]);
		}
	}

	render_cursor_to_swapbuffer();
	for (int idx = 0; idx < SUPPORTED_WIDTH * SUPPORTED_HEIGHT; idx++) {
		framebuffer[idx] = data[idx];
	}
}

char window_contains(InternalWindow* window, int x, int y) {
	return rect_contains(window->x, window->y, window->width, window->height, x, y);
}

void window_resolve_click(InternalWindow* window, int x, int y) {
	// X button
	if (rect_contains((TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2, x, y)) {
		window->present = 0;
	}
}

void resolve_click(int x, int y) {
	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present && window_contains(&windows[i], x, y)) {
			window_resolve_click(&windows[i], x - windows[i].x, y - windows[i].y);
		}
	}
}

void update_cursor() {
	MousePacket packet;
	unsigned length = read(mouse_socket, &packet, sizeof(MousePacket));
	
	while (length != 0) {
		assert(length == sizeof(MousePacket));
		mouse_x = clamp(mouse_x + packet.x_delta, 0, info.width);
		mouse_y = clamp(mouse_y - packet.y_delta, 0, info.height);

		if (packet.flags.left_button) {
			resolve_click(mouse_x, mouse_y);
		}

		length = read(mouse_socket, &packet, sizeof(MousePacket));
	}
}

void handle_request(unsigned action, unsigned sender, void* raw) {
	switch (action) {
		case WSCreateWindow: {
			CreateWindowRequest* createwindow = (CreateWindowRequest*) raw;

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
			return;
		}
		case WSDestroyWindow: {
			DestroyWindowRequest* req = (DestroyWindowRequest*) raw;
			
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
			return;
		}
		case WSUpdateElement: {
			WindowServerElementUpdateRequest* req = (WindowServerElementUpdateRequest*) raw;
			WindowServerElementUpdateResponse res;

			if ((int) req->elementId == -1) {
				switch (req->elementType) {
					case WSLabelElement: {
						WindowServerLabelUpdateRequest* labelreq = (WindowServerLabelUpdateRequest*) raw;

						if (req->window >= WINDOWS_CAPACITY) {
							debugf("Invalid window ID\n");
							return;
						}
						InternalWindow* window = &windows[req->window];
						if (window->creatorpid != sender) {
							debugf("Invalid permissions for window\n");
							return;
						}

						if (!window->present) {
							debugf("Window does not exist\n");
							return;
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

										res.elementId = i;
										break;
									}
								}
								break;
							}
						}
						break;
					}
					default: {
						debugf("Unknown element type 0x%.8x\n", req->elementType);
						return;
					}
				}
			} else {
				switch (req->elementType) {
					case WSLabelElement: {
						WindowServerLabelUpdateRequest* labelreq = (WindowServerLabelUpdateRequest*) raw;

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

						if (req->elementId >= WINDOW_ELEMENTS_CAPACITY) {
							debugf("Invalid element ID (does not exist)\n");
							return;
						}
						
						InternalLabelElement* element = (InternalLabelElement*) window->elements[req->elementId];
						if (element->type != WSLabelElement) {
							debugf("Not a label\n");
							return;
						}

						if (!element->present) {
							debugf("Invalid element ID (not present)\n");
							return;
						}

						memcpy(element->content, labelreq->content, 256);
						element->color = labelreq->color;
						element->x = labelreq->x;
						element->y = labelreq->y;
						element->scale = labelreq->scale;
						res.elementId = element->elementID;
						render_window(window);
						break;
					}
					default: {
						debugf("Unknown element type 0x%.8x\n", req->elementType);
						return;
					}
				}
			}

			send_ipc_message(sender, &res, sizeof(res));
			return;
		}
		case WSWindowStatus: {
			WindowStatusRequest* req = (WindowStatusRequest*) raw;
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

			send_ipc_message(sender, &res, sizeof(WindowStatusResponse));
			return;
		}
		default: {
			debugf("Unknown command %d\n", action);
			return;
		}
	}
}

char rawrequest[1024];
void recieve_messages() {
	unsigned sender;


	while (1) {
		update_cursor();
		
		int status = read_ipc_message(rawrequest, 1024, &sender);
		while (status >= 0) {
			WindowServerAction action = ((WindowServerRequest*) rawrequest)->action;
			handle_request(action, sender, rawrequest);
			memset(rawrequest, 0, 1024);

			status = read_ipc_message(rawrequest, 1024, &sender);
		}
		
		render();
	}
}

int main() {
	info.enabled = 1;
	framebuffer_set_state(&info);

	assert(info.width == SUPPORTED_WIDTH);
	assert(info.height == SUPPORTED_HEIGHT);
	// Why 2 times? Is the second half a swapbuffer?
	assert(info.size == SUPPORTED_SIZE * sizeof(unsigned) * 2);

	framebuffer = (Pixel*) info.framebuffer;

	desktopBackground = pixel_from_hex(0xffffff);

	mouse_socket = open("/dev/mouse", FILE_FLAG_R | FILE_FLAG_SOCK);

	render();

	exec("/usr/bin/guiapp");

	recieve_messages();

	return 0;
}
