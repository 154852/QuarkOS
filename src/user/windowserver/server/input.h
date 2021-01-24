#include "window.h"

#ifndef _WINDOWSERVER_INPUT_H
#define _WINDOWSERVER_INPUT_H

#define rect_contains(rx, ry, rw, rh, x, y) ((x) >= (rx) && (x) <= ((rx) + (rw)) && (y) >= (ry) && (y) <= ((ry) + (rh)))

void initialise_mouse();
void initialise_keyboard();

char window_contains(InternalWindow* window, int x, int y);
void window_resolve_click(InternalWindow* window, int x, int y);
void resolve_click(int x, int y);
void update_cursor();

int get_mouse_x();
int get_mouse_y();

void update_keyboard();

#endif