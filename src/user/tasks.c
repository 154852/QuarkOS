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
ProcessInfo info;

char* state_string(ProcessStateSC state) {
	switch (state) {
		case PSSC_NotPresent: return "EXIT";
		case PSSC_Exitting: return "EXIT";
		case PSSC_Running: return "RUN ";
		case PSSC_Idle: return "IDLE";
		default: return "????";
	}
}

int main() {
	windowhandle = create_window("Tasks", WINWIDTH, 400, 100, 100, 1);

	memcpy(entries[0].name, "name", 5);

	int count = read_dir("/dev/proc/", entries, 256);
	if (count == -EFILENOTFOUND) {
		debugf("File not found\n");
		exit(1);
	}

	create_rectangle(windowhandle, PADDING, PADDING, WINWIDTH - PADDING*2, ROWH - PADDING, pixel_from_rgba(0xff, 0xff, 0xff, 0x80));
	create_label(windowhandle, "PID - Name - State", 0, TEXTPADDINGX, TEXTPADDINGY);

	for (int i = 0; i < count; i++) {
		if (entries[i].type == FT_Socket) {
			int fd = open(entries[i].name, FILE_FLAG_R | FILE_FLAG_SOCK);
			read(fd, &info, sizeof(info));

			char string[128];
			char pid[16]; itoa(info.pid, pid, 10);
			int pidlen = strlen(pid);
			memcpy(string, pid, pidlen);
			memcpy(string + pidlen, " - ", 3);

			int namelen = strlen(info.name);
			memcpy(string + pidlen + 3, info.name, namelen);
			memcpy(string + pidlen + 3 + namelen, " - ", 3);
			memcpy(string + pidlen + 3 + namelen + 3, state_string(info.state), 4);
			string[pidlen + 3 + namelen + 3 + 4] = 0;

			create_rectangle(windowhandle, PADDING, (i * ROWH) + PADDING + 35, WINWIDTH - PADDING*2, ROWH - PADDING, pixel_from_rgba(0xff, 0xff, 0xff, 0x80));
			create_label(windowhandle, string, 0, TEXTPADDINGX, (i * ROWH) + TEXTPADDINGY + 35);
		}
	}
	
	mainloop(windowhandle);

	return 0;
}
