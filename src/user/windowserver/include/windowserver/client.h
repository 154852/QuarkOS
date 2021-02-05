#ifndef _WINDOWSERVER_CLIENT_H
#define _WINDOWSERVER_CLIENT_H

#include <windowserver/wsmsg.h>

typedef unsigned WindowHandle;
typedef unsigned ElementID;

unsigned get_windowserver_pid();

WindowHandle create_window(char* title, unsigned width, unsigned height, unsigned x, unsigned y, char has_title_bar);
WindowHandle create_window_detailed(char* title, unsigned width, unsigned height, unsigned x, unsigned y, char has_title_bar, Pixel background);
void destroy_window(WindowHandle handle);
void render_window(WindowHandle handle);

ElementID update_label(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y);
ElementID update_label_detailed(unsigned windowid, unsigned id, const char* content, Pixel* color, int x, int y, float scale);
ElementID create_label(unsigned windowid, const char* content, Pixel* color, int x, int y);

ElementID update_button(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, Pixel* background);
ElementID create_button(unsigned windowid, int x, int y, unsigned width, unsigned height, Pixel* background);

ElementID update_rectangle(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, Pixel background);
ElementID create_rectangle(unsigned windowid, int x, int y, unsigned width, unsigned height, Pixel background);

ImageLoadResponse load_image(unsigned windowid, const char* path);
ElementID update_image(unsigned windowid, unsigned id, int x, int y, unsigned width, unsigned height, int image_id);
ElementID create_image(unsigned windowid, int x, int y, unsigned width, unsigned height, int image_id);

#endif