#ifndef _WINDOWSERVER_BUFFER_H
#define _WINDOWSERVER_BUFFER_H

#include <windowserver/color.h>

void initialise_buffers();

Pixel* get_swapbuffer();
Pixel* get_framebuffer();

#endif