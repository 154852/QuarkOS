#include "../color.h"

#ifndef _WINDOWSERVER_COLOR_MACOS_H
#define _WINDOWSERVER_COLOR_MACOS_H

#define DEFAULT_TEXT_COLOR (Pixel) { .r = 0x0, .g = 0x0, .b = 0x0, .a = 0xff }
#define DEFAULT_WINDOW_COLOR (Pixel) { .r = 0xff, .g = 0xff, .b = 0xff, .a = 0xff }
#define COLOR_SECONDARY_BACKGROUND pixel_from_rgb(0xf0, 0xf0, 0xf0)

#endif