#include "windowserver/color.h"
#include <windowserver/image.h>
#include <stdlib.h>
#include <string.h>

Bitmap* load_bmp(const char* path) {
	Bitmap* bitmap = malloc(sizeof(Bitmap));
	memset(bitmap, 0, sizeof(Bitmap));
	bitmap->width = 10;
	bitmap->height = 10;
	bitmap->data = malloc(bitmap->width * bitmap->height * sizeof(Pixel));
	for (int i = 0; i < bitmap->width * bitmap->height; i++) {
		((Pixel*) bitmap->data)[i] = pixel_from_rgb(0, 0xff, 0);
	}
	return bitmap;
}