#ifndef _WINDOWSERVER_RENDER_H
#define _WINDOWSERVER_RENDER_H

#include "window.h"

void render_cursor_to_swapbuffer();
void render_window_to_swapbuffer(InternalWindow* window);
void render();

#endif