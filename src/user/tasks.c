#include "windowserver/color.h"
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>

WindowHandle windowhandle;
ElementID labelID;

#define PADDING 5
#define TEXTPADDINGX 12
#define TEXTPADDINGY 8
#define ROWH 35
#define WINWIDTH 600

DirEntry entries[256];

int main() {
	windowhandle = create_window("Tasks", WINWIDTH, 400, 100, 100, 1);

	int count = read_dir("/proc/", entries, 256);
	if (count == -EFILENOTFOUND) {
		debugf("File not found\n");
		exit(1);
	}

	create_rectangle(windowhandle, PADDING, PADDING, WINWIDTH - PADDING*2, ROWH - PADDING, pixel_from_rgba(0xff, 0xff, 0xff, 0x80));
	create_label(windowhandle, "PID - Name - State", 0, TEXTPADDINGX, TEXTPADDINGY);

	for (int i = 0; i < count; i++) {
		if (entries[i].type == FT_Directory) {
			char string[128]; memset(string, 0, sizeof(string));
			int pidlen = strlen(entries[i].name);
			memcpy(string, entries[i].name, pidlen);
			memcpy(string + pidlen, " - ", 3);

			char path[128]; memset(path, 0, sizeof(path));
			memcpy(path, entries[i].name, pidlen);
			path[pidlen] = '/';
			memcpy(path + pidlen + 1, "exe", 4);

			int fd = open(path, FILE_FLAG_R);

			char name[128]; memset(name, 0, sizeof(name));
			read(fd, name, sizeof(name));

			int namelen = strlen(name);
			memcpy(string + pidlen + 3, name, namelen);
			memcpy(string + pidlen + 3 + namelen, " - ", 3);

			memcpy(path + pidlen + 1, "status", 7);
			fd = open(path, FILE_FLAG_R | FILE_FLAG_SOCK);

			char state[10]; memset(state, 0, sizeof(state));
			read(fd, state, sizeof(state));

			int statelen = strlen(name);
			memcpy(string + pidlen + 3 + namelen + 3, state, statelen);

			create_rectangle(windowhandle, PADDING, (i * ROWH) + PADDING + 35, WINWIDTH - PADDING*2, ROWH - PADDING, pixel_from_rgba(0xff, 0xff, 0xff, 0x80));
			create_label(windowhandle, string, 0, TEXTPADDINGX, (i * ROWH) + TEXTPADDINGY + 35);
		}
	}
	
	mainloop(windowhandle);

	return 0;
}
