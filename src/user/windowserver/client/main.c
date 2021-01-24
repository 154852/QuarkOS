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

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(CreateWindowRequest));

	CreateWindowResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(CreateWindowResponse), &senderpid);

		if (status < 0) continue;

		if (senderpid != get_windowserver_pid()) {
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

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(DestroyWindowRequest));
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

		if (status < 0) continue;

		if (senderpid != get_windowserver_pid()) {
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

	send_ipc_message(get_windowserver_pid(), (char*) &req, sizeof(WindowServerLabelUpdateRequest));

	WindowServerElementUpdateResponse res;
	unsigned senderpid;

	while (1) {
		int status = read_ipc_message((char*) &res, sizeof(WindowServerElementUpdateResponse), &senderpid);

		if (status < 0) continue;

		if (senderpid != get_windowserver_pid()) {
			debugf("Unexpected message from pid = %d\n", senderpid);
			exit(1);
		}
		
		return res.elementId;
	}
}

ElementID create_label(unsigned windowid, const char* content, Pixel* color, int x, int y) {
	return update_label(windowid, -1, content, color, x, y);
}