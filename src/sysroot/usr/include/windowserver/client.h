#ifndef _WINDOWSERVER_CLIENT_H
#define _WINDOWSERVER_CLIENT_H

#include <windowserver/wsmsg.h>

typedef unsigned WindowHandle;
typedef unsigned ElementID;

unsigned get_windowserver_pid();

WindowHandle create_window(char* title, unsigned width, unsigned height, unsigned x, unsigned y, char has_title_bar);
void destroy_window(WindowHandle handle);

WindowStatusResponse query_status(WindowHandle handle);

ElementID update_label(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y);
ElementID create_label(unsigned windowid, const char* content, Pixel* color, int x, int y);

ElementID update_button(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, Pixel* background);
ElementID create_button(unsigned windowid, int x, int y, unsigned width, unsigned height, Pixel* background);

#endif