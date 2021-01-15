#include "wsmsg.h"
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <string.h>
#include "fontchars.h"

#define SUPPORTED_WIDTH 1024
#define SUPPORTED_HEIGHT 768
#define SUPPORTED_SIZE (SUPPORTED_WIDTH * SUPPORTED_HEIGHT)
#define FULL_SIZE (SUPPORTED_SIZE * sizeof(unsigned))

#define max(a, b) ((a) > (b)? (a):(b))
#define min(a, b) ((a) < (b)? (a):(b))

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
unsigned* framebuffer;

void blit() {
	memcpy(framebuffer, data, FULL_SIZE);
	memset(data, 0, FULL_SIZE);
}

int idx_for_xy(int x, int y) {
	return ((y * info.width) + x);
}

int idx_for_xyw(int x, int y, int w) {
	return ((y * w) + x);
}

int mix(int a, int b, float frac) {
	return ((b - a) * frac) + a;
}

void copy_image(int x0, int y0, Pixel* image, int w, int h, double scale, const Pixel* color) {
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int image_idx = idx_for_xyw(x, y, w);
			int framebuffer_idx = idx_for_xy((x * scale) + x0, (y * scale) + y0);

			Pixel pixel = image[image_idx];
			if (color != 0) pixel = *color;

			if (image[image_idx].a == 0xff) {
				data[framebuffer_idx] = pixel;
			} else if (image[image_idx].a != 0) {
				float frac = (float) image[image_idx].a / (float) 0xff;
				data[framebuffer_idx].r = mix(data[framebuffer_idx].r, pixel.r, frac);
				data[framebuffer_idx].g = mix(data[framebuffer_idx].g, pixel.g, frac);
				data[framebuffer_idx].b = mix(data[framebuffer_idx].b, pixel.b, frac);
			}
			data[framebuffer_idx].a = 0xff;
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

	InternalElement* elements[WINDOW_ELEMENTS_CAPACITY];
} InternalWindow;

#define WINDOWS_CAPACITY 32
static InternalWindow windows[WINDOWS_CAPACITY];
#define LABELS_CAPACITY 32
static InternalLabelElement labelElements[LABELS_CAPACITY];

#define WINDOW_BUTTON_SIZE 15

void render_label(InternalWindow* window, InternalLabelElement* label) {
	int x = window->x + label->x;
	int y = window->y + TITLE_BAR_HEIGHT + label->y;
	double scale = 1;
	for (size_t i = 0; i < strlen(label->content); i++) {
		FontChar chr = fontchar_for_char(label->content[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &label->color);
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

			int idx = idx_for_xy(x + window->x, y + window->y);
			if (x == 0 || x == (int) window->width - 1 || y == (int) window->height - 1 || y == TITLE_BAR_HEIGHT || y == 0) {
				data[idx] = COLOR_VERYLIGHTGREY;
			} else if (y < TITLE_BAR_HEIGHT) {
				data[idx] = COLOR_LIGHTGREY;
			} else {
				data[idx].raw = window->background.raw;
			}
		}
	}

	for (int x = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; x < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; x++) {
		for (int y = (TITLE_BAR_HEIGHT - WINDOW_BUTTON_SIZE) / 2; y < (TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2; y++) {
			int idx = idx_for_xy(x + window->x, y + window->y);
			data[idx] = COLOR_DARKRED;
		}
	}

	FontChar chr = fontchar_for_char('A');
	double paddingpc = 0.6;
	double scale = (TITLE_BAR_HEIGHT * (1.0 - paddingpc)) / chr.height;
	int padding = TITLE_BAR_HEIGHT * paddingpc * 0.5;

	int x = window->x + ((TITLE_BAR_HEIGHT + WINDOW_BUTTON_SIZE) / 2) + padding;
	int y = window->y + padding;
	for (size_t i = 0; i < strlen(window->title); i++) {
		FontChar chr = fontchar_for_char(window->title[i]);
		if (chr.raw != 0) copy_image(x, y, (Pixel*) chr.raw, chr.width, chr.height, scale, &COLOR_DARKGREY);
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

void render() {
	for (int i = 0; i < WINDOWS_CAPACITY; i++) {
		if (windows[i].present) {
			render_window(&windows[i]);
		}
	}
	for (int x = 0; x < info.width; x++) {
		for (int y = 0; y < info.height; y++) {
			int idx = idx_for_xy(x, y);
			if (data[idx].a == 0) {
				data[idx].raw = desktopBackground.raw;
			}
		}
	}
	blit();
}

void handle_request(unsigned action, unsigned sender, char* raw) {
	switch (action) {
		case WSCreateWindow: {
			CreateWindowRequest* createwindow = (CreateWindowRequest*) raw;

			CreateWindowResponse res;
			for (int i = 0; i < WINDOWS_CAPACITY; i++) {
				if (!windows[i].present) {
					InternalWindow* win = &windows[i];

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
					break;
				}
			}

			send_ipc_message(sender, (char*) &res, sizeof(res));
			render();
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
				debugf("Cannot (yet) update elements\n");
				return;
			}

			send_ipc_message(sender, (char*) &res, sizeof(res));
			render();
			return;
		}
		default: {
			debugf("Unknown command %d\n", action);
			return;
		}
	}
}

char rawrequest[1024];
void recieve_message() {
	unsigned sender;
	
	while (1) {
		int status = read_ipc_message(rawrequest, 1024, &sender);
		if (status < 0) continue;

		WindowServerAction action = ((WindowServerRequest*) rawrequest)->action;
		handle_request(action, sender, rawrequest);
		memset(rawrequest, 0, 1024);
	}
}

int main() {
	info.enabled = 1;
	framebuffer_set_state(&info);

	assert(info.width == SUPPORTED_WIDTH);
	assert(info.height == SUPPORTED_HEIGHT);
	assert(info.size == SUPPORTED_SIZE * sizeof(unsigned) * 2);

	framebuffer = info.framebuffer;

	desktopBackground = pixel_from_hex(0xffffff);

	render();

	exec("sysroot/usr/bin/guiapp");

	while (1) {
		recieve_message();
	}

	hang;
	return 0;
}
