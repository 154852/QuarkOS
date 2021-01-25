#include <windowserver/image.h>

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
				float frac = (float) image[image_idx].a / (float) 0xff;
				out[framebuffer_idx].r = mix(out[framebuffer_idx].r, pixel.r, frac);
				out[framebuffer_idx].g = mix(out[framebuffer_idx].g, pixel.g, frac);
				out[framebuffer_idx].b = mix(out[framebuffer_idx].b, pixel.b, frac);
			}
			out[framebuffer_idx].a = 0xff;
		}
	}
}