#include <stdint2.h>
#include <stdbool.h>

#ifndef _KERNEL_KEYBOARD_SCANCODES_H
#define _KERNEL_KEYBOARD_SCANCODES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_KEYPAD_0, KEY_KEYPAD_1, KEY_KEYPAD_2, KEY_KEYPAD_3, KEY_KEYPAD_4, KEY_KEYPAD_5, KEY_KEYPAD_6, KEY_KEYPAD_7, KEY_KEYPAD_8, KEY_KEYPAD_9,
	KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	KEY_DASH, KEY_EQUALS, KEY_SEMI_COLON, KEY_SINGLE_QUOTE, KEY_BACK_TICK, KEY_BACK_SLASH, KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_KEYPAD_STAR, KEY_LEFT_ALT, KEY_SPACE, KEY_KEYPAD_DOT,
	KEY_ESCAPE, KEY_BACKSPACE, KEY_TAB, KEY_ENTER, KEY_LEFT_CONTROL, KEY_RIGHT_SHIFT, KEY_CAPS_LOCK, KEY_LEFT_SHIFT, KEY_NUMBER_LOCK, KEY_SCROLL_LOCK,
	KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	KEY_UNKNOWN,
} KeyName;

typedef enum {
	KEY_PRESS,
	KEY_RELEASE,
	KEY_UNKNOWN_ACTION,
} KeyAction;

typedef struct {
	KeyAction action;
	KeyName name;
} ScanCode;

const ScanCode* scan_code(unsigned char code);

#ifdef __cplusplus
}
#endif

#endif