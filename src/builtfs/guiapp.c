#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include "wsmsg.h"
#include "softfloat.h"

unsigned windowserver;
typedef unsigned WindowHandle;

WindowHandle create_window(char* title, unsigned width, unsigned height, unsigned x, unsigned y) {
	CreateWindowRequest req;
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

int main() {
	debugf("GUI APP STARTED\n");
	windowserver = find_proc_pid("sysroot/usr/bin/windowserver");

	WindowHandle windowhandle = create_window("Hello World!", 400, 300, 100, 100);
	debugf("WindowHandle = %d\n", windowhandle);

	// int ix = 10;
	// debugf("ix = %d, sizeof(double)=%d\n", ix, sizeof(double));

	double x = (1.5 + windowhandle) * 3.0;
	// uint64_t result = f64_add((float64_t) { *(uint64_t*) &x }, i32_to_f64((int32_t) { ix })).v;
	// x = *(double*) &result;
	debugf("%f\n", x);

	return 0;
}
