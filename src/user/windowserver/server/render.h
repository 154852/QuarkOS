#ifndef _WINDOWSERVER_RENDER_H
#define _WINDOWSERVER_RENDER_H

#include "window.h"
#include "themes/theme.h"

void render_cursor_to_swapbuffer();
void render_window_to_swapbuffer(InternalWindow* window);
void render();

Theme* get_theme();
void set_theme(Theme theme);

#endif