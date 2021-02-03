#include "windowserver/client.h"
#include <windowserver/mainloop.h>
#include <syscall.h>
#include <stdio.h>

OnClickListener onclicklisteners[ONCLICK_LISTENERS_CAPACITY];

OnKeyPressListener onkeypresslisteners[ONKEYPRESS_LISTENERS_CAPACITY];

static char should_close;

void set_should_close(char should_close_) {
	should_close = should_close_;
}

void mainloop(WindowHandle windowhandle) {
	render_window(windowhandle);

	WindowStatusResponse res;
	res = query_status(windowhandle);
	while (res.present && !should_close) {
		res = query_status(windowhandle);

		if (res.last_event.present) {
			switch (res.last_event.type) {
				case WSEvButtonClick: {
					for (int i = 0; i < ONCLICK_LISTENERS_CAPACITY; i++) {
						if (onclicklisteners[i].present && onclicklisteners[i].buttonID == res.last_event.element) {
							onclicklisteners[i].cb(onclicklisteners[i].id);
						}
					}
					break;
				}
				case WSEvKeyPress: {
					for (int i = 0; i < ONKEYPRESS_LISTENERS_CAPACITY; i++) {
						if (onkeypresslisteners[i].present) {
							onkeypresslisteners[i].cb(&((WindowServerKeyboardEvent*) &res.last_event)->event);
						}
					}
					break;
				}
				default: {
					debugf("Unknown event type: %d\n", res.last_event.type);
					break;
				}
			}
		}
	}

	if (res.present) {
		destroy_window(windowhandle);
	}
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