#include "../color.h"

#ifndef _WINDOWSERVER_COLOR_GRAY_H
#define _WINDOWSERVER_COLOR_GRAY_H

#define DEFAULT_TEXT_COLOR (Pixel) { .r = 0x0, .g = 0x0, .b = 0x0, .a = 0xff }
#define DEFAULT_WINDOW_COLOR (Pixel) { .r = 0xf0, .g = 0xf0, .b = 0xf0, .a = 0xff }
#define COLOR_SECONDARY_BACKGROUND pixel_from_rgb(0xe0, 0xe0, 0xe0)

#endif