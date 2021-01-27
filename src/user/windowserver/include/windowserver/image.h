#ifndef _WINDOWSERVER_IMAGE_H
#define _WINDOWSERVER_IMAGE_H

#include <windowserver/color.h>
#include <windowserver/config.h>

#define idx_for_xy(x, y) (((int) (y) * SUPPORTED_WIDTH) + (int) (x))
#define idx_for_xyw(x, y, w) (((int) (y) * (int) (w)) + (int) (x))

void copy_image(int x0, int y0, Pixel* image, int w, int h, double scale, const Pixel* color, Pixel* out, int memw);
void antialias(Pixel* image, int w, int h, int x, int y, int memw, int memh);

#endif