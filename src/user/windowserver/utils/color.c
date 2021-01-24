#include <windowserver/color.h>

Pixel pixel_from_hex(unsigned hex) {
	if (hex > 0xffffff) {
		return (Pixel) {
			.b = hex & 0xff,
			.g = (hex >> 8) & 0xff,
			.r = (hex >> 16) & 0xff,
			.a = (hex >> 24) & 0xff
		};
	}

	if (hex > 0xfff) {
		return (Pixel) {
			.b = hex & 0xff,
			.g = (hex >> 8) & 0xff,
			.r = (hex >> 16) & 0xff,
			.a = 0xff
		};
	}

	return (Pixel) {
		.b = (hex & 0xf) * 0xf,
		.g = ((hex >> 4) & 0xf) * 0xf,
		.r = ((hex >> 12) & 0xf) * 0xf,
		.a = 0xff
	};
}