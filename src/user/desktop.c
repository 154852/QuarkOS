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
	exec(dockApps[x].path);
}

void open_app(int x) {
	exec(dockApps[x].path);
}

#define APPBAR_HEIGHT 50
#define OPENAPPWIDTH 300
#define OPENAPPPADDINGX 7
#define OPENAPPPADDINGY 7
#define APPSELECTW (OPENAPPWIDTH * 2)
#define APPSELECTH 600
#define APPSELECTAPPH 45
#define APPSELECTPADDING 7
#define TIMEPADDING 10

int string_width(float scale, const char* str) {
	int width = 0;
	for (size_t i = 0; i < strlen(str); i++) {
		width += (fontchar_for_char(str[i]).width * scale) + 1;
	}
	return width;
}

int string_height(float scale) {
	return FONTCHAR_61_H * scale;
}

void to2digits(unsigned value, char* str) {
	if (value < 10) {
		str[0] = '0';
		str[1] = '0' + value;
	} else {
		itoa(value, str, 10);
	}
}

int main() {
	windowhandle = create_window_detailed("Desktop", SUPPORTED_WIDTH, SUPPORTED_HEIGHT, 0, 0, 0, pixel_from_rgba(243, 22, 145, 0xff));

	dockApps[0] = (DockApp) {
		.name="Font Viewer",
		.path="/usr/bin/font",
	};

	dockApps[1] = (DockApp) {
		.name="Calculator",
		.path="/usr/bin/calc",
	};

	dockApps[2] = (DockApp) {
		.name="Tasks",
		.path="/usr/bin/tasks",
	};

	ImageLoadResponse image = load_image(windowhandle, "system/wallpaper.bmp");
	create_image(windowhandle, 0, 0, image.width, image.height, image.id);

	create_rectangle(windowhandle, 0, SUPPORTED_HEIGHT - APPBAR_HEIGHT, SUPPORTED_WIDTH, APPBAR_HEIGHT, pixel_from_rgba(0xff, 0xff, 0xff, 0xc0));

	update_label_detailed(windowhandle, -1, "Q", &COLOR_BLACK, APPBAR_HEIGHT*0.35, SUPPORTED_HEIGHT - APPBAR_HEIGHT/2 - 15, 0.7);
	create_rectangle(windowhandle, APPBAR_HEIGHT, SUPPORTED_HEIGHT - APPBAR_HEIGHT + 7, 1, APPBAR_HEIGHT - 14, pixel_from_rgba(0x00, 0x00, 0x00, 0x40));

	int x = APPBAR_HEIGHT * 1.2;
	for (int i = 0; i < 3; i++) {
		ElementID button = create_button(windowhandle, x, SUPPORTED_HEIGHT - APPBAR_HEIGHT + OPENAPPPADDINGY, OPENAPPWIDTH, APPBAR_HEIGHT - OPENAPPPADDINGY*2, &pixel_from_rgba(0xff, 0xff, 0xff, 0xa0));
		onclick(button, i, focus_app);
		create_label(windowhandle, dockApps[i].name, &COLOR_BLACK, x + 10, SUPPORTED_HEIGHT - APPBAR_HEIGHT/2 - 10);
	
		x += OPENAPPWIDTH + OPENAPPPADDINGX;
	}

	FullTime timeraw; get_full_time(&timeraw);
	
	char time[6];
	to2digits(timeraw.hour, time);
	time[2] = ':';
	to2digits(timeraw.minute, time + 3);
	time[5] = 0;

	char date[15];

	switch (timeraw.weekday) {
		case 1: {
			memcpy(date, "Sun", 3);
			break;
		}
		case 2: {
			memcpy(date, "Mon", 3);
			break;
		}
		case 3: {
			memcpy(date, "Tue", 3);
			break;
		}
		case 4: {
			memcpy(date, "Wed", 3);
			break;
		}
		case 5: {
			memcpy(date, "Thu", 3);
			break;
		}
		case 6: {
			memcpy(date, "Fri", 3);
			break;
		}
		case 7: {
			memcpy(date, "Sat", 3);
			break;
		}
		default: {
			memcpy(date, "???", 3);
			break;
		}
	}

	date[3] = ' ';
	to2digits(timeraw.date, date + 4);
	date[6] = '/';
	to2digits(timeraw.month, date + 7);
	date[9] = '/';
	itoa(timeraw.year, date + 10, 10);
	date[14] = 0;
	
	update_label_detailed(windowhandle, -1, time, &pixel_from_rgb(0xff, 0xff, 0xff), SUPPORTED_WIDTH - TIMEPADDING - string_width(1.7, time), 0, 1.7);
	update_label_detailed(windowhandle, -1, date, &pixel_from_rgb(0xff, 0xff, 0xff), SUPPORTED_WIDTH - TIMEPADDING - string_width(0.45, date) - 5, string_height(1.7), 0.45);
	

	mainloop(windowhandle);

	return 0;
}
