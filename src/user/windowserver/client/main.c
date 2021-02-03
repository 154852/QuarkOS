#include "windowserver/themes/color-active.h"
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <ckeyboard.h>

unsigned get_windowserver_pid() {
	static int pid = -1;
	if (pid == -1) {
		pid = find_proc_pid("/usr/bin/windowserver");
		assert((int) pid != -ENOTFOUND);
	}
	return pid;
}

WindowHandle create_window_detailed(char* title, unsigned width, unsigned height, unsigned x, unsigned y, char has_title_bar, Pixel background) {
	CreateWindowRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSCreateWindow;
	size_t titlelen = strlen(title);
	memcpy(req.title, title, titlelen > 63? 63:titlelen);
	req.width = width;
	req.height = height;
	req.x = x;
	req.y = y;
	req.has_title_bar = has_title_bar;
	req.background = background;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(CreateWindowRequest));

	CreateWindowResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(CreateWindowResponse), &senderpid);

		if (status < 0) {
			yield(); // TODO: Does yield here actually improve performance? Or just take time from the next process to run?
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.handle;
	}
}

WindowHandle create_window(char* title, unsigned width, unsigned height, unsigned x, unsigned y, char has_title_bar) {
	return create_window_detailed(title, width, height, x, y, has_title_bar, DEFAULT_WINDOW_COLOR);
}

void destroy_window(WindowHandle handle) {
	DestroyWindowRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSDestroyWindow;
	req.window = handle;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(DestroyWindowRequest));
}

ImageLoadResponse load_image(unsigned windowid, const char* path) {
	ImageLoadRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSLoadImage;
	size_t pathlen = strlen(path);
	memcpy(req.path, path, pathlen > 63? 63:pathlen);

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(ImageLoadRequest));

	ImageLoadResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(ImageLoadResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res;
	}
}

WindowStatusResponse query_status(WindowHandle handle) {
	WindowStatusRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSWindowStatus;
	req.window = handle;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowStatusRequest));

	WindowStatusResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowStatusResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res;
	}
}

ElementID update_label_detailed(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y, float scale) {
	WindowServerLabelUpdateRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSUpdateElement;
	req.elementId = id;
	req.elementType = WSLabelElement;
	req.scale = scale;
	size_t titlelen = strlen(content);
	memcpy(req.content, content, titlelen > 255? 255:titlelen);
	req.x = x;
	req.y = y;
	req.color = color == 0? DEFAULT_TEXT_COLOR:*color;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowServerLabelUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID update_label(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y) {
	return update_label_detailed(windowid, id, content, color, x, y, 0.5);
}

ElementID create_label(unsigned windowid, const char* content, Pixel* color, int x, int y) {
	return update_label(windowid, -1, content, color, x, y);
}

ElementID update_button(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, Pixel* background) {
	WindowServerButtonUpdateRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSUpdateElement;
	req.elementId = id;
	req.elementType = WSButtonElement;
	req.x = x;
	req.y = y;
	req.width = width;
	req.height = height;
	req.background = background == 0? (Pixel) { .r = 0x0, .g = 0x0, .b = 0x0, .a = 0xff }:*background;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowServerButtonUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID create_button(unsigned windowid, int x, int y, unsigned width, unsigned height, Pixel* background) {
	return update_button(windowid, -1, x, y, width, height, background);
}

ElementID update_rectangle(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, Pixel background) {
	WindowServerRectangleUpdateRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSUpdateElement;
	req.elementId = id;
	req.elementType = WSRectangle;
	req.x = x;
	req.y = y;
	req.width = width;
	req.height = height;
	req.background = background;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowServerRectangleUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID create_rectangle(unsigned windowid, int x, int y, unsigned width, unsigned height, Pixel background) {
	return update_rectangle(windowid, -1, x, y, width, height, background);
}

ElementID update_image(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, int image_id) {
	WindowServerImageUpdateRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSUpdateElement;
	req.elementId = id;
	req.elementType = WSImageElement;
	req.x = x;
	req.y = y;
	req.width = width;
	req.height = height;
	req.image_id = image_id;

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowServerImageUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) {
			yield();
			continue;
		}

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID create_image(unsigned windowid, int x, int y, unsigned width, unsigned height, int image_id) {
	return update_image(windowid, -1, x, y, width, height, image_id);
}