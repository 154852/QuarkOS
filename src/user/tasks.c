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

DirEntry entries[256];
ProcessInfo info;

int main() {
	windowhandle = create_window("Tasks", 600, 400, 100, 100, 1);

	memcpy(entries[0].name, "name", 5);

	int count = read_dir("/dev/proc/", entries, 256);
	if (count == -EFILENOTFOUND) {
		debugf("File not found\n");
		exit(1);
	}

	for (int i = 0; i < count; i++) {
		if (entries[i].type == FT_Socket) {
			int fd = open(entries[i].name, FILE_FLAG_R | FILE_FLAG_SOCK);
			read(fd, &info, sizeof(info));
			create_label(windowhandle, info.name, 0, 5, (i * 25) + 5);
		}
	}
	
	mainloop(windowhandle);

	return 0;
}
