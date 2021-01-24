#ifndef _WINDOWSERVER_WINDOW_H
#define _WINDOWSERVER_WINDOW_H

#include "elements.h"

#define TITLE_BAR_HEIGHT 30
#define WINDOW_BUTTON_SIZE 15

InternalWindow* get_windows();
InternalLabelElement* get_label_elements();

void render_label(InternalWindow* window, InternalLabelElement* label);
void render_window(InternalWindow* window);

#endif