#include "windowserver/color.h"
#include "windowserver/config.h"
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>

typedef struct {
	char name[64];
	char path[64];
	Pixel color;
	void(*cb)();
} DockApp;

#define DOCK_APP_COUNT 3
DockApp dockApps[DOCK_APP_COUNT];

ElementID buttons[DOCK_APP_COUNT];
ElementID labels[DOCK_APP_COUNT];

WindowHandle windowhandle;

#define WINDOW_HEIGHT 50
#define BUTTON_PADDING 15
#define BUTTON_HEIGHT 30

void open_guiapp() {
	exec(dockApps[0].path);
}

int main() {
	windowhandle = create_window("Dock", SUPPORTED_WIDTH, WINDOW_HEIGHT, 0, SUPPORTED_HEIGHT - WINDOW_HEIGHT, 0);

	Pixel darkgray = pixel_from_rgb(0xe0, 0xe0, 0xe0);

	dockApps[0] = (DockApp) {
		.name="guiapp",
		.path="/usr/bin/guiapp",
		.color=darkgray,
		.cb=open_guiapp
	};

	dockApps[1] = (DockApp) {
		.name="also guiapp",
		.path="/usr/bin/guiapp",
		.color=darkgray,
		.cb=open_guiapp
	};

	dockApps[2] = (DockApp) {
		.name="also guiapp2",
		.path="/usr/bin/guiapp",
		.color=darkgray,
		.cb=open_guiapp
	};

	int width = ((SUPPORTED_WIDTH - (2*BUTTON_PADDING)) / DOCK_APP_COUNT) - BUTTON_PADDING;
	int y = (WINDOW_HEIGHT - BUTTON_HEIGHT) / 2;

	for (int i = 0; i < DOCK_APP_COUNT; i++) {
		int x = BUTTON_PADDING + (i * (width + BUTTON_PADDING));
		buttons[i] = create_button(windowhandle, x, y, width, BUTTON_HEIGHT, (Pixel*) &dockApps[i].color);
		labels[i] = create_label(windowhandle, dockApps[i].name, 0, x + 5, 20);
		onclick(buttons[i], dockApps[i].cb);
	}

	mainloop(windowhandle);

	return 0;
}
