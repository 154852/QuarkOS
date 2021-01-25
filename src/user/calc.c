#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>

WindowHandle windowhandle;

// void onclick() {
// 	memcpy(text, "Clicked", 8);
// 	textidx = 7;
// 	update_label(windowhandle, labelID, text, 0, 5, 5);
// }

int main() {
	// windowhandle = create_window("Hello World!", 400, 300, 100, 100, 1);
	// button = create_button(windowhandle, 100, 100, 50, 20, &COLOR_RED);
	// labelID = create_label(windowhandle, text, 0, 5, 5);
	
	// onkeydown(keypress);
	// onclick(button, onclickme);

	// mainloop(windowhandle);

	return 0;
}
