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
	WindowServerEventType type;
	unsigned element;
} InternalEvent;

typedef struct {
	char present;
	unsigned handle;
	unsigned creatorpid;

	char title[64];
	unsigned width;
	unsigned height;
	int x;
	int y;

	InternalEvent events[WINDOW_EVENTS_CAPACITY];

	Pixel background;
	Pixel raster[SUPPORTED_SIZE];

	InternalElement* elements[WINDOW_ELEMENTS_CAPACITY];
} InternalWindow;

#endif