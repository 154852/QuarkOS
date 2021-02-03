#include <ckeyboard.h>
#include <windowserver/color.h>
#include <windowserver/config.h>

#ifndef _WINDOWSERVER_WSMSG_H
#define _WINDOWSERVER_WSMSG_H

typedef enum {
	WSCreateWindow,
	WSDestroyWindow,
	WSUpdateElement,
	WSWindowStatus,
	WSLoadImage
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
	WSEvButtonClick,
	WSEvKeyPress
} WindowServerEventType;

#define EVENT_DATA_SIZE 32

typedef struct {
	char present;
	unsigned element;
	WindowServerEventType type;
	char zero[EVENT_DATA_SIZE];
} WindowServerEvent;

typedef struct {
	char present;
	unsigned element;
	WindowServerEventType type;

	KeyEvent event;
	char zero[EVENT_DATA_SIZE - sizeof(KeyEvent)];
} WindowServerKeyboardEvent;

typedef struct {
	char present;

	WindowServerEvent last_event;

	int x;
	int y;
	unsigned width;
	unsigned height;
} WindowStatusResponse;

typedef struct {
	WindowServerAction action;
	unsigned window;

	char path[64];
} ImageLoadRequest;

typedef struct {
	char success;
	int id;
	int width;
	int height;
} ImageLoadResponse;

typedef enum {
	WSLabelElement,
	WSButtonElement,
	WSRectangle,
	WSImageElement
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
} WindowServerRectangleUpdateRequest;

typedef struct {
	WindowServerAction action;
	WindowServerElementType elementType;
	unsigned window;
	unsigned elementId;
	
	int x;
	int y;
	unsigned width;
	unsigned height;
	int image_id;
} WindowServerImageUpdateRequest;

#endif