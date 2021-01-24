#include <windowserver/wsmsg.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <string.h>
#include <windowserver/fontchars.h>
#include <windowserver/client.h>
#include "buffer.h"
#include "elements.h"
#include "window.h"
#include "render.h"
#include "request.h"
#include "input.h"
#include "windowserver/color.h"
#include <windowserver/image.h>
#include <windowserver/config.h>

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
	initialise_buffers();
	initialise_mouse();
	render();

	exec("/usr/bin/guiapp");

	recieve_messages();

	return 0;
}
