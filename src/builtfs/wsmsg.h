typedef enum {
	WSCreateWindow,
	WSDestroyWindow,
	WSUpdateElement,
	WSWindowStatus
} WindowServerAction;

typedef union {
	struct {
		unsigned char b;
		unsigned char g;
		unsigned char r;
		unsigned char a;
	};

	unsigned raw;
} Pixel;

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

typedef struct {
	char present;
	int x;
	int y;
	unsigned width;
	unsigned height;
} WindowStatusResponse;

typedef enum {
	WSLabelElement,
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