#ifndef _CMOUSE_H
#define _CMOUSE_H

typedef union {
	struct __attribute__((packed)) {
		unsigned char left_button : 1;
		unsigned char right_button : 1;
		unsigned char middle_button : 1;
		unsigned char always_1 : 1;
		unsigned char x_sign : 1;
		unsigned char y_sign : 1;
		unsigned char x_overflow : 1;
		unsigned char y_overflow : 1;
	};

	unsigned char raw;
} MousePacketFlags;

typedef struct {
	MousePacketFlags flags;
	int x_delta;
	int y_delta;
	int scroll_delta_x;
	int scroll_delta_y;
}  MousePacket;

#endif