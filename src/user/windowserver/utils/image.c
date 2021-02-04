#include "windowserver/color.h"
#include <windowserver/image.h>
#include <stdio.h>

// TODO: We really need anti-aliasing
void copy_image(int x0, int y0, Pixel* image, int w, int h, double scale, const Pixel* color, Pixel* out, int memw) {
	int xs = w * scale;
	int ys = h * scale;

	float invscale = 1.0 / scale;

	for (int x = 0; x < xs; x++) {
		for (int y = 0; y < ys; y++) {
			int framebuffer_idx = idx_for_xy(x + x0, y + y0);
			int imx = x * invscale;
			int imy = y * invscale;

			int image_idx = idx_for_xyw(imx, imy, memw);

			Pixel pixel = image[image_idx];
			if (color != 0) pixel = *color;

			if (image[image_idx].a == 0xff) {
				out[framebuffer_idx] = pixel;
			} else if (image[image_idx].a != 0) {
				unsigned char a = image[image_idx].a;
				out[framebuffer_idx].r = mixi(mixi(pixel.r, out[framebuffer_idx].r, out[framebuffer_idx].a), pixel.r, a);
				out[framebuffer_idx].g = mixi(mixi(pixel.g, out[framebuffer_idx].g, out[framebuffer_idx].a), pixel.g, a);
				out[framebuffer_idx].b = mixi(mixi(pixel.b, out[framebuffer_idx].b, out[framebuffer_idx].a), pixel.b, a);
				out[framebuffer_idx].a = out[framebuffer_idx].a + ((0xff - out[framebuffer_idx].a) * a)/0xff;
			}
		}
	}
}

void blur(Pixel* image, int w, int h, int x0, int y0, int square_diameter, int memw, int memh) {
	(void) memh;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int r = 0;
			int g = 0;
			int b = 0;
			int count = 0;

			for (int xr = 0; xr <= square_diameter; xr++) {
				for (int yr = 0; yr <= square_diameter; yr++) {
					int xo = xr + x + x0 - (square_diameter / 2);
					int yo = yr + y + y0 - (square_diameter / 2);
					if (xo < 0 || xo >= x0 + w || yo < 0 || yo >= y0 + h) continue;

					Pixel pixel = image[idx_for_xyw(xo, yo, memw)];
					r += pixel.r;
					g += pixel.g;
					b += pixel.b;
					count++;
				}
			}

			r /= count;
			g /= count;
			b /= count;

			image[idx_for_xyw(x + x0, y + y0, memw)] = pixel_from_rgb(r, g, b);
		}
	}
}

void antialias(Pixel* image, int w, int h, int x, int y, int memw, int memh) {
	blur(image, w, h, x, y, 1, memw, memh);
}