#include <windowserver/color.h>
#include <windowserver/config.h>

#ifndef _WINDOWSERVER_WSMSG_H
#define _WINDOWSERVER_WSMSG_H

typedef enum {
	WSCreateWindow,
	WSDestroyWindow,
	WSUpdateElement,
	WSWindowStatus
} WindowServerAction;

typedef struct {
	WindowServerAction action;
} WindowServerRequest;

typedef struct {
	WindowServerAction action;
	char title[64];
	unsigned width;
	unsigned height;
	int x;
	int y;
	Pixel background;
	char has_title_bar;
} CreateWindowRequest;

typedef struct {
	unsigned handle;
} CreateWindowResponse;

typedef struct {
	WindowServerAction action;
	unsigned window;
} DestroyWindowRequest;

typedef struct {
	WindowServerAction action;
	unsigned window;
} WindowStatusRequest;

typedef enum {
	WSEvButtonClick
} WindowServerEventType;

typedef struct {
	char present;
	unsigned element;
	WindowServerEventType type;
} WindowServerEvent;

typedef struct {
	char present;

	WindowServerEvent last_event;

	int x;
	int y;
	unsigned width;
	unsigned height;
} WindowStatusResponse;

typedef enum {
	WSLabelElement,
	WSButtonElement
} WindowServerElementType;

typedef struct {
	WindowServerAction action;
	WindowServerElementType elementType;
	unsigned window;
	unsigned elementId;
	/* element data */
} WindowServerElementUpdateRequest;

typedef struct {
	unsigned elementId;
} WindowServerElementUpdateResponse;


typedef struct {
	WindowServerAction action;
	WindowServerElementType elementType;
	unsigned window;
	unsigned elementId;
	
	char content[256];
	int x;
	int y;
	Pixel color;
	float scale;
} WindowServerLabelUpdateRequest;

typedef struct {
	WindowServerAction action;
	WindowServerElementType elementType;
	unsigned window;
	unsigned elementId;
	
	int x;
	int y;
	unsigned width;
	unsigned height;
	Pixel background;
} WindowServerButtonUpdateRequest;

#endif