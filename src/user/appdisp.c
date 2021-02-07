#include "windowserver/color.h"
#include "windowserver/fontchars.h"
#include "windowserver/themes/color-active.h"
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
} DockApp;

#define DOCK_APP_COUNT 4
DockApp dockApps[DOCK_APP_COUNT];

ElementID buttons[DOCK_APP_COUNT];
ElementID labels[DOCK_APP_COUNT];

WindowHandle windowhandle;

void focus_app(int x) {
	debugf("// TODO: Allow for window focusing\n");
	exec(dockApps[x].path, 0, 0);
}

void open_app(int x) {
	exec(dockApps[x].path, 0, 0);
}

#define APPBAR_HEIGHT 50
#define OPENAPPWIDTH 300
#define OPENAPPPADDINGX 7
#define OPENAPPPADDINGY 7
#define APPSELECTW (OPENAPPWIDTH * 2)
#define APPSELECTH 600
#define APPSELECTAPPH 45
#define APPSELECTPADDING 7

int main() {
	int height = SUPPORTED_HEIGHT/4;
	windowhandle = create_window_detailed("App Disp", SUPPORTED_WIDTH, height, 0, (SUPPORTED_HEIGHT*3)/4, 0, pixel_from_rgba(0, 0, 0, 0));

	dockApps[0] = (DockApp) {
		.name="Shell",
		.path="/usr/bin/shell",
	};

	dockApps[1] = (DockApp) {
		.name="Calculator",
		.path="/usr/bin/calc",
	};

	dockApps[2] = (DockApp) {
		.name="Tasks",
		.path="/usr/bin/tasks",
	};

	create_rectangle(windowhandle, 0, height - APPBAR_HEIGHT, SUPPORTED_WIDTH, APPBAR_HEIGHT, pixel_from_rgba(0xff, 0xff, 0xff, 0xc0));

	update_label_detailed(windowhandle, -1, "Q", &COLOR_BLACK, APPBAR_HEIGHT*0.35, height - APPBAR_HEIGHT/2 - 15, 0.7);
	create_rectangle(windowhandle, APPBAR_HEIGHT, height - APPBAR_HEIGHT + 7, 1, APPBAR_HEIGHT - 14, pixel_from_rgba(0x00, 0x00, 0x00, 0x40));

	int x = APPBAR_HEIGHT * 1.2;
	for (int i = 0; i < 3; i++) {
		ElementID button = create_button(windowhandle, x, height - APPBAR_HEIGHT + OPENAPPPADDINGY, OPENAPPWIDTH, APPBAR_HEIGHT - OPENAPPPADDINGY*2, &pixel_from_rgba(0xff, 0xff, 0xff, 0xa0));
		onclick(button, i, focus_app);
		create_label(windowhandle, dockApps[i].name, &COLOR_BLACK, x + 10, height - APPBAR_HEIGHT/2 - 10);
	
		x += OPENAPPWIDTH + OPENAPPPADDINGX;
	}	

	mainloop(windowhandle);

	return 0;
}
