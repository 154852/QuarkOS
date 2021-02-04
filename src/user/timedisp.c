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

WindowHandle windowhandle;

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
	int width = SUPPORTED_WIDTH/2;
	windowhandle = create_window_detailed("TimeDisp", width, SUPPORTED_HEIGHT/3, SUPPORTED_WIDTH/2, 0, 0, pixel_from_rgba(0, 0, 0, 0));

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
	
	update_label_detailed(windowhandle, -1, time, &pixel_from_rgb(0xff, 0xff, 0xff), width - TIMEPADDING - string_width(1.7, time), 0, 1.7);
	update_label_detailed(windowhandle, -1, date, &pixel_from_rgb(0xff, 0xff, 0xff), width - TIMEPADDING - string_width(0.45, date) - 5, string_height(1.7), 0.45);
	

	mainloop(windowhandle);

	return 0;
}
