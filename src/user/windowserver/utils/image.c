#include <windowserver/image.h>

void copy_image(int x0, int y0, Pixel* image, int w, int h, double scale, const Pixel* color, Pixel* out, int memw) {
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			int image_idx = idx_for_xyw(x, y, memw);
			int framebuffer_idx = idx_for_xy((x * scale) + x0, (y * scale) + y0);

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