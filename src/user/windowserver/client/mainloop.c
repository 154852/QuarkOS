#include "windowserver/client.h"
#include <windowserver/mainloop.h>
#include <assertions.h>
#include <syscall.h>
#include <stdio.h>

OnClickListener onclicklisteners[ONCLICK_LISTENERS_CAPACITY];
OnKeyPressListener onkeypresslisteners[ONKEYPRESS_LISTENERS_CAPACITY];

static char should_close;

char windowservereventmsg[1024];

void set_should_close(char should_close_) {
	should_close = should_close_;
}

void mainloop(WindowHandle windowhandle) {
	mainloop_cb(windowhandle, 0);
}

void mainloop_cb(WindowHandle windowhandle, void(*cb)()) {
	render_window(windowhandle);
	
	unsigned sender;
	while (!should_close) {
		int status = read_ipc_message(windowservereventmsg, sizeof(windowservereventmsg), &sender);
		while (status > 0) {
			WindowServerEvent* rawevent = (WindowServerEvent*) windowservereventmsg;
			assert(rawevent->windowid == windowhandle);
			assert(sender == get_windowserver_pid());

			switch (rawevent->type) {
				case WSEvButtonClick: {
					WindowServerButtonClickEvent* event = (WindowServerButtonClickEvent*) windowservereventmsg;

					for (int i = 0; i < ONCLICK_LISTENERS_CAPACITY; i++) {
						if (onclicklisteners[i].present && onclicklisteners[i].buttonID == event->element) {
							onclicklisteners[i].cb(onclicklisteners[i].id);
						}
					}
					break;
				}
				case WSEvKeyPress: {
					WindowServerKeyboardEvent* event = (WindowServerKeyboardEvent*) windowservereventmsg;

					for (int i = 0; i < ONKEYPRESS_LISTENERS_CAPACITY; i++) {
						if (onkeypresslisteners[i].present) {
							onkeypresslisteners[i].cb(&event->event);
						}
					}
					break;
				}
				case WSEvDestroy: {
					return;
				}
				default: {
					debugf("Unknown event type: %d\n", rawevent->type);
					break;
				}
			}

			status = read_ipc_message(windowservereventmsg, sizeof(windowservereventmsg), &sender);
		}

		if (cb != 0) cb();

		// yield();
	}

	destroy_window(windowhandle);
}

void onkeydown(void(*cb)(KeyEvent* event)) {
	for (int i = 0; i < ONKEYPRESS_LISTENERS_CAPACITY; i++) {
		if (!onkeypresslisteners[i].present) {
			onkeypresslisteners[i].present = 1;
			onkeypresslisteners[i].cb = cb;
			return;
		}
	}

	debugf("Out of onkeydown capacity\n");
}

void onclick(ElementID buttonID, int id, void(*cb)(int id)) {
	for (int i = 0; i < ONCLICK_LISTENERS_CAPACITY; i++) {
		if (!onclicklisteners[i].present) {
			onclicklisteners[i].present = 1;
			onclicklisteners[i].buttonID = buttonID;
			onclicklisteners[i].id = id;
			onclicklisteners[i].cb = cb;
			return;
		}
	}

	debugf("Out of onclick capacity\n");
}