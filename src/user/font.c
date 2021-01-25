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

const char* alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+=*/\?()[]{}'\"<>,.#!";

int main() {
	windowhandle = create_window("Font", 820, 60, 100, 100, 1);
	labelID = create_label(windowhandle, alpha, 0, 5, 5);
	
	mainloop(windowhandle);

	return 0;
}
