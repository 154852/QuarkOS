#ifndef _WINDOWSERVER_COLOR_H
#define _WINDOWSERVER_COLOR_H

typedef union {
	struct {
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};

	unsigned raw;
} Pixel;

#define max(a, b) ((a) > (b)? (a):(b))
#define min(a, b) ((a) < (b)? (a):(b))
#define clamp(x, a, b) min(max(x, a), b)
#define mix(a, b, frac) ((((float) (b) - (float) (a)) * (float) (frac)) + (float) (a))

Pixel pixel_from_hex(unsigned hex);

#define pixel_from_rgb(_r, _g, _b) (Pixel) { .r = _r, .g = _g, .b = _b, .a = 0xff }
#define pixel_from_rgba(_r, _g, _b, _a) (Pixel) { .r = _r, .g = _g, .b = _b, .a = _a }

#define COLOR_BLACK pixel_from_rgb(0, 0, 0)
#define COLOR_DARKGREY pixel_from_rgb(0x60, 0x60, 0x60)
#define COLOR_VERYDARKGREY pixel_from_rgb(0x40, 0x40, 0x40)
#define COLOR_LIGHTGREY pixel_from_rgb(0xe0, 0xe0, 0xe0)
#define COLOR_VERYLIGHTGREY pixel_from_rgb(0xc0, 0xc0, 0xc0)
#define COLOR_RED pixel_from_rgb(0xff, 0, 0)
#define COLOR_DARKRED pixel_from_rgb(0xd0, 0, 0)

#endif