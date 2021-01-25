#ifndef _WINDOWSERVER_MAINLOOP_H
#define _WINDOWSERVER_MAINLOOP_H

#include <windowserver/client.h>
#include <ckeyboard.h>

#define ONCLICK_LISTENERS_CAPACITY 32
#define ONKEYPRESS_LISTENERS_CAPACITY 32

typedef struct {
	char present;
	ElementID buttonID;
	int id;
	void(*cb)(int id);
} OnClickListener;

typedef struct {
	char present;
	void(*cb)(KeyEvent* event);
} OnKeyPressListener;

void mainloop(WindowHandle windowhandle);
void set_should_close(char should_close);

void onkeydown(void(*cb)(KeyEvent* event));
void onclick(ElementID buttonID, int id, void(*cb)(int id));

#endif