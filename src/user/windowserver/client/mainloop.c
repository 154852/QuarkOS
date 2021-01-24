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
	unsigned fd = open("/dev/keyboard", FILE_FLAG_R | FILE_FLAG_SOCK);
	char keydata[sizeof(KeyEvent)];

	WindowStatusResponse res;
	res = query_status(windowhandle);
	while (res.present && !should_close) {
		unsigned len = read(fd, keydata, sizeof(KeyEvent));
		while (len != 0) {
			KeyEvent* state = (KeyEvent*) keydata;

			for (int i = 0; i < ONKEYPRESS_LISTENERS_CAPACITY; i++) {
				if (onkeypresslisteners[i].present) {
					onkeypresslisteners[i].cb(state);
				}
			}

			len = read(fd, keydata, sizeof(KeyEvent));
		}

		res = query_status(windowhandle);

		if (res.last_event.present) {
			switch (res.last_event.type) {
				case WSEvButtonClick: {
					for (int i = 0; i < ONCLICK_LISTENERS_CAPACITY; i++) {
						if (onclicklisteners[i].present && onclicklisteners[i].buttonID == res.last_event.element) {
							onclicklisteners[i].cb();
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
}

void onclick(ElementID buttonID, void(*cb)()) {
	for (int i = 0; i < ONCLICK_LISTENERS_CAPACITY; i++) {
		if (!onclicklisteners[i].present) {
			onclicklisteners[i].present = 1;
			onclicklisteners[i].buttonID = buttonID;
			onclicklisteners[i].cb = cb;
			return;
		}
	}
}