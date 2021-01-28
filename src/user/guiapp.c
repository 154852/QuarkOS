// NOTE: This was just a file to test gui capabilities

#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>


char text[64] = "a";
int textidx = 1;

WindowHandle windowhandle;
ElementID labelID;
ElementID button;

void keypress(KeyEvent* state) {
	if (state->action == KEY_PRESS && state->name == KEY_BACKSPACE) {
		text[textidx - 1] = 0;
		textidx--;
	} else if (state->action == KEY_PRESS && state->name == KEY_C && state->is_ctrl) {
		set_should_close(1);
	} else {
		char chr = scan_code_to_char(state);
		if (chr == 0) return;
		text[textidx++] = chr;
	}
	
	update_label(windowhandle, labelID, text, 0, 5, 5);
}

void onclickme(int x) {
	(void) x;
	memcpy(text, "Clicked", 8);
	textidx = 7;
	update_label(windowhandle, labelID, text, 0, 5, 5);
}

int main() {
	windowhandle = create_window("Hello World!", 400, 300, 100, 100, 1);
	button = create_button(windowhandle, 100, 100, 50, 20, &COLOR_RED);
	labelID = create_label(windowhandle, text, 0, 5, 5);
	
	onkeydown(keypress);
	onclick(button, 0, onclickme);

	mainloop(windowhandle);

	return 0;
}
