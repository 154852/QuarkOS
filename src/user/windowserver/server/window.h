#ifndef _WINDOWSERVER_WINDOW_H
#define _WINDOWSERVER_WINDOW_H

#include "elements.h"

#define TITLE_BAR_HEIGHT 30
#define WINDOW_BUTTON_SIZE 15

void destroy_internal_window(InternalWindow* window);

InternalWindow* get_windows();
InternalLabelElement* get_label_elements();
InternalButtonElement* get_button_elements();

WindowServerEvent* allocate_event(InternalWindow* window);

void render_label(InternalWindow* window, InternalLabelElement* label);
void render_button(InternalWindow* window, InternalButtonElement* button);
void render_window(InternalWindow* window);

InternalWindow* get_focused();
void set_focused(InternalWindow* window);

#define window_title_bar_height(window) ((window)->has_title_bar? TITLE_BAR_HEIGHT:0)

#endif