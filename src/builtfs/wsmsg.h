typedef enum {
	WSCreateWindow
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