#include "windowserver/config.h"
#include "windowserver/image.h"
#include <windowserver/wsmsg.h>

#ifndef _WINDOWSERVER_ELEMENTS_H
#define _WINDOWSERVER_ELEMENTS_H

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
} InternalElement;

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
	
	char content[256];
	int x;
	int y;
	Pixel color;
	float scale;
} InternalLabelElement;

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
	
	int x;
	int y;
	unsigned width;
	unsigned height;
	Pixel background;
} InternalButtonElement;

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
	
	int x;
	int y;
	unsigned width;
	unsigned height;
	Pixel background;
} InternalRectangleElement;

typedef struct {
	char present;
	WindowServerElementType type;
	unsigned elementID;
	
	int x;
	int y;
	unsigned width;
	unsigned height;
	int image_id;
} InternalImageElement;

typedef struct {
	unsigned handle;
	unsigned creatorpid;

	char has_title_bar;

	char title[64];
	unsigned width;
	unsigned height;
	int x;
	int y;

	Pixel background;
	Pixel raster[SUPPORTED_SIZE];

	InternalElement* elements[WINDOW_ELEMENTS_CAPACITY];
	Bitmap* bitmaps[WINDOW_BITMAPS_CAPACITY];
} InternalWindow;

#endif