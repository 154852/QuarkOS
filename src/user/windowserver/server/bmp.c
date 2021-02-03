#include "syscall.h"
#include "windowserver/color.h"
#include <windowserver/image.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char header[54];

#define read32(data, off) *(unsigned int*) &header[off]
#define read16(data, off) *(unsigned short*) &header[off]

Bitmap* load_bmp(const char* path) {
	unsigned handle = open(path, FILE_FLAG_R);
	if (handle == 0) {
		debugf("Could not open file\n");
		return 0;
	}

	read(handle, header, sizeof(header));
	if (header[0] != 'B' || header[1] != 'M') {
		debugf("Not a BMP\n");
		return 0;
	}

	size_t file_size = read32(header, 2);
	size_t bitmap_start = read32(header, 10);

	size_t width = read32(header, 18);
	size_t height = read32(header, 22);

	short bpp = read16(header, 28);
	if (bpp != 32) {
		debugf("Expected 32 bpp\n", bpp);
		return 0;
	}

	unsigned compression = read32(header, 30);
	if (compression != 3) {
		debugf("Expected BI_BITFIELDS (no) compression.\n");
		return 0;
	}

	size_t bitmap_size = width * height * sizeof(Pixel);

	if (file_size - bitmap_start != bitmap_size) {
		debugf("Invalid file size, expected %d, found %d\n", bitmap_size, bitmap_start);
		return 0;
	}

	char* full_file = malloc(file_size);
	read(handle, full_file, file_size);

	Bitmap* bitmap = malloc(sizeof(Bitmap));
	memset(bitmap, 0, sizeof(Bitmap));

	bitmap->width = width;
	bitmap->height = height;
	bitmap->data = malloc(bitmap_size);
	memcpy(bitmap->data, full_file + bitmap_start, bitmap_size);
	
	free_sized(full_file, file_size);
	debugf("[WindowServer] Image %s loaded\n", path);

	return bitmap;
}