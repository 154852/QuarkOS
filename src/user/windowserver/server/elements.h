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

#define WINDOW_ELEMENTS_CAPACITY 32
typedef struct {
	char present;
	unsigned handle;
	unsigned creatorpid;

	char title[64];
	unsigned width;
	unsigned height;
	int x;
	int y;

	Pixel background;
	Pixel raster[SUPPORTED_SIZE];

	InternalElement* elements[WINDOW_ELEMENTS_CAPACITY];
} InternalWindow;

#endif