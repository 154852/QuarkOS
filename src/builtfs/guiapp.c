#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include "assertions.h"
#include "wsmsg.h"
#include <kernel/ckeyboard.h>

unsigned windowserver;
typedef unsigned WindowHandle;
typedef unsigned ElementID;

WindowHandle create_window(char* title, unsigned width, unsigned height, unsigned x, unsigned y) {
	CreateWindowRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSCreateWindow;
	size_t titlelen = strlen(title);
	memcpy(req.title, title, titlelen > 64? 64:titlelen);
	req.width = width;
	req.height = height;
	req.x = x;
	req.y = y;
	req.background = (Pixel) { .r = 0xf0, .g = 0xf0, .b = 0xf0, .a = 0xff };

	send_ipc_message(windowserver, (char*) &req, sizeof(CreateWindowRequest));

	CreateWindowResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(CreateWindowResponse), &senderpid);

		if (status < 0) continue;

		if (senderpid != windowserver) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.handle;
	}
}

void destroy_window(WindowHandle handle) {
	DestroyWindowRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSDestroyWindow;
	req.window = handle;

	send_ipc_message(windowserver, (char*) &req, sizeof(DestroyWindowRequest));
}

WindowStatusResponse query_status(WindowHandle handle) {
	WindowStatusRequest req;
	memset(&req, 0, sizeof(req));
	req.action = WSWindowStatus;
	req.window = handle;

	send_ipc_message(windowserver, (char*) &req, sizeof(WindowStatusRequest));

	WindowStatusResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowStatusResponse), &senderpid);

		if (status < 0) continue;

		if (senderpid != windowserver) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res;
	}
}

ElementID update_label(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y) {
	WindowServerLabelUpdateRequest req;
	memset(&req, 0, sizeof(req));
	req.window = windowid;
	req.action = WSUpdateElement;
	req.elementId = id;
	req.elementType = WSLabelElement;
	req.scale = 0.5;
	size_t titlelen = strlen(content);
	memcpy(req.content, content, titlelen > 256? 256:titlelen);
	req.x = x;
	req.y = y;
	req.color = color == 0? (Pixel) { .r = 0x0, .g = 0x0, .b = 0x0, .a = 0xff }:*color;

	send_ipc_message(windowserver, (char*) &req, sizeof(WindowServerLabelUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) continue;

		if (senderpid != windowserver) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID create_label(unsigned windowid, const char* content, Pixel* color, int x, int y) {
	return update_label(windowid, -1, content, color, x, y);
}

int main() {
	windowserver = find_proc_pid("/usr/bin/windowserver");
	assert((int) windowserver != -ENOTFOUND);

	WindowHandle windowhandle = create_window("Hello World!", 400, 300, 100, 100);

	char text[64] = "a";
	int textidx = 1;
	ElementID labelID = create_label(windowhandle, text, 0, 5, 5);

	unsigned fd = open("/dev/keyboard", FILE_FLAG_R | FILE_FLAG_SOCK);
	char data[sizeof(KeyEvent)];
	while (query_status(windowhandle).present) {
		unsigned len = read(fd, data, sizeof(KeyEvent));
		if (len != 0) {
			KeyEvent* state = (KeyEvent*) data;
			if (state->action == KEY_PRESS && state->name == KEY_BACKSPACE) {
				text[textidx - 1] = 0;
				textidx--;
			} else if (state->action == KEY_PRESS && state->name == KEY_C && state->is_ctrl) {
				destroy_window(windowhandle);
				break;
			} else {
				char chr = scan_code_to_char(state);
				if (chr == 0) continue;
				text[textidx++] = chr;
			}
			
			update_label(windowhandle, labelID, text, 0, 5, 5);
		}
	}

	return 0;
}
