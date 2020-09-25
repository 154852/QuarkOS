#include <kernel/hardware/keyboard_scancodes.hpp>

namespace Keyboard {
	#define key_unknown { KEY_UNKNOWN_ACTION, KEY_UNKNOWN }
	static const ScanCode scan_codes[] = {
			key_unknown,
		/* 0x01: */
			{ KEY_PRESS, KEY_ESCAPE },
		/* 0x02: */
			{ KEY_PRESS, KEY_1 }, { KEY_PRESS, KEY_2 }, { KEY_PRESS, KEY_3 }, { KEY_PRESS, KEY_4 }, { KEY_PRESS, KEY_5 }, { KEY_PRESS, KEY_6 }, { KEY_PRESS, KEY_7 }, { KEY_PRESS, KEY_8 }, { KEY_PRESS, KEY_9 }, { KEY_PRESS, KEY_0 },
		/* 0x0C: */
			{ KEY_PRESS, KEY_DASH }, { KEY_PRESS, KEY_EQUALS }, { KEY_PRESS, KEY_BACKSPACE }, { KEY_PRESS, KEY_TAB },
		/* 0x10: */
			{ KEY_PRESS, KEY_Q }, { KEY_PRESS, KEY_W }, { KEY_PRESS, KEY_E }, { KEY_PRESS, KEY_R }, { KEY_PRESS, KEY_T }, { KEY_PRESS, KEY_Y }, { KEY_PRESS, KEY_U }, { KEY_PRESS, KEY_I }, { KEY_PRESS, KEY_O }, { KEY_PRESS, KEY_P },
		/* 0x1A: */
			{ KEY_PRESS, KEY_LEFT_BRACKET }, { KEY_PRESS, KEY_RIGHT_BRACKET }, { KEY_PRESS, KEY_ENTER }, { KEY_PRESS, KEY_LEFT_CONTROL },
		/* 0x1E: */
			{ KEY_PRESS, KEY_A }, { KEY_PRESS, KEY_S }, { KEY_PRESS, KEY_D }, { KEY_PRESS, KEY_F }, { KEY_PRESS, KEY_G }, { KEY_PRESS, KEY_H }, { KEY_PRESS, KEY_J }, { KEY_PRESS, KEY_K }, { KEY_PRESS, KEY_L },
		/* 0x27: */
			{ KEY_PRESS, KEY_SEMI_COLON }, { KEY_PRESS, KEY_SINGLE_QUOTE }, { KEY_PRESS, KEY_BACK_TICK }, { KEY_PRESS, KEY_LEFT_SHIFT }, { KEY_PRESS, KEY_BACK_SLASH }, 
		/* 0x2C: */
			{ KEY_PRESS, KEY_Z }, { KEY_PRESS, KEY_X }, { KEY_PRESS, KEY_C }, { KEY_PRESS, KEY_V }, { KEY_PRESS, KEY_B }, { KEY_PRESS, KEY_N }, { KEY_PRESS, KEY_M }, 
		/* 0x33: */
			{ KEY_PRESS, KEY_COMMA }, { KEY_PRESS, KEY_DOT }, { KEY_PRESS, KEY_SLASH }, { KEY_PRESS, KEY_RIGHT_SHIFT }, { KEY_PRESS, KEY_KEYPAD_STAR }, { KEY_PRESS, KEY_LEFT_ALT }, { KEY_PRESS, KEY_SPACE }, { KEY_PRESS, KEY_CAPS_LOCK }, 
		/* 0x3B: */
			{ KEY_PRESS, KEY_F1 }, { KEY_PRESS, KEY_F2 }, { KEY_PRESS, KEY_F3 }, { KEY_PRESS, KEY_F4 }, { KEY_PRESS, KEY_F5 }, { KEY_PRESS, KEY_F6 }, { KEY_PRESS, KEY_F7 }, { KEY_PRESS, KEY_F8 }, { KEY_PRESS, KEY_F9 }, { KEY_PRESS, KEY_F10 },
		/* 0x45: */
			{ KEY_PRESS, KEY_NUMBER_LOCK }, { KEY_PRESS, KEY_SCROLL_LOCK },
		/* 0x47: */
			{ KEY_PRESS, KEY_KEYPAD_1 }, { KEY_PRESS, KEY_KEYPAD_2 }, { KEY_PRESS, KEY_KEYPAD_3 }, { KEY_PRESS, KEY_KEYPAD_4 }, { KEY_PRESS, KEY_KEYPAD_5 }, { KEY_PRESS, KEY_KEYPAD_6 }, { KEY_PRESS, KEY_KEYPAD_7 }, { KEY_PRESS, KEY_KEYPAD_8 }, { KEY_PRESS, KEY_KEYPAD_9 }, { KEY_PRESS, KEY_KEYPAD_0 },
		/* 0x53: */
			{ KEY_PRESS, KEY_KEYPAD_DOT }, key_unknown, key_unknown, key_unknown, { KEY_PRESS, KEY_F11 }, { KEY_PRESS, KEY_F12 },

			key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown,
			key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown,
			key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown, key_unknown,  key_unknown, 

		/* 0x81: (same, but action = KEY_RELEASE) */
			{ KEY_RELEASE, KEY_ESCAPE },
			{ KEY_RELEASE, KEY_1 }, { KEY_RELEASE, KEY_2 }, { KEY_RELEASE, KEY_3 }, { KEY_RELEASE, KEY_4 }, { KEY_RELEASE, KEY_5 }, { KEY_RELEASE, KEY_6 }, { KEY_RELEASE, KEY_7 }, { KEY_RELEASE, KEY_8 }, { KEY_RELEASE, KEY_9 }, { KEY_RELEASE, KEY_0 },
			{ KEY_RELEASE, KEY_DASH }, { KEY_RELEASE, KEY_EQUALS }, { KEY_RELEASE, KEY_BACKSPACE }, { KEY_RELEASE, KEY_TAB },
			{ KEY_RELEASE, KEY_Q }, { KEY_RELEASE, KEY_W }, { KEY_RELEASE, KEY_E }, { KEY_RELEASE, KEY_R }, { KEY_RELEASE, KEY_T }, { KEY_RELEASE, KEY_Y }, { KEY_RELEASE, KEY_U }, { KEY_RELEASE, KEY_I }, { KEY_RELEASE, KEY_O }, { KEY_RELEASE, KEY_P },
			{ KEY_RELEASE, KEY_LEFT_BRACKET }, { KEY_RELEASE, KEY_RIGHT_BRACKET }, { KEY_RELEASE, KEY_ENTER }, { KEY_RELEASE, KEY_LEFT_CONTROL },
			{ KEY_RELEASE, KEY_A }, { KEY_RELEASE, KEY_S }, { KEY_RELEASE, KEY_D }, { KEY_RELEASE, KEY_F }, { KEY_RELEASE, KEY_G }, { KEY_RELEASE, KEY_H }, { KEY_RELEASE, KEY_J }, { KEY_RELEASE, KEY_K }, { KEY_RELEASE, KEY_L },
			{ KEY_RELEASE, KEY_SEMI_COLON }, { KEY_RELEASE, KEY_SINGLE_QUOTE }, { KEY_RELEASE, KEY_BACK_TICK }, { KEY_RELEASE, KEY_LEFT_SHIFT }, { KEY_RELEASE, KEY_BACK_SLASH }, 
			{ KEY_RELEASE, KEY_Z }, { KEY_RELEASE, KEY_X }, { KEY_RELEASE, KEY_C }, { KEY_RELEASE, KEY_V }, { KEY_RELEASE, KEY_B }, { KEY_RELEASE, KEY_N }, { KEY_RELEASE, KEY_M }, 
			{ KEY_RELEASE, KEY_COMMA }, { KEY_RELEASE, KEY_DOT }, { KEY_RELEASE, KEY_SLASH }, { KEY_RELEASE, KEY_RIGHT_SHIFT }, { KEY_RELEASE, KEY_KEYPAD_STAR }, { KEY_RELEASE, KEY_LEFT_ALT }, { KEY_RELEASE, KEY_SPACE }, { KEY_RELEASE, KEY_CAPS_LOCK }, 
			{ KEY_RELEASE, KEY_F1 }, { KEY_RELEASE, KEY_F2 }, { KEY_RELEASE, KEY_F3 }, { KEY_RELEASE, KEY_F4 }, { KEY_RELEASE, KEY_F5 }, { KEY_RELEASE, KEY_F6 }, { KEY_RELEASE, KEY_F7 }, { KEY_RELEASE, KEY_F8 }, { KEY_RELEASE, KEY_F9 }, { KEY_RELEASE, KEY_F10 },
			{ KEY_RELEASE, KEY_NUMBER_LOCK }, { KEY_RELEASE, KEY_SCROLL_LOCK },
			{ KEY_RELEASE, KEY_KEYPAD_1 }, { KEY_RELEASE, KEY_KEYPAD_2 }, { KEY_RELEASE, KEY_KEYPAD_3 }, { KEY_RELEASE, KEY_KEYPAD_4 }, { KEY_RELEASE, KEY_KEYPAD_5 }, { KEY_RELEASE, KEY_KEYPAD_6 }, { KEY_RELEASE, KEY_KEYPAD_7 }, { KEY_RELEASE, KEY_KEYPAD_8 }, { KEY_RELEASE, KEY_KEYPAD_9 }, { KEY_RELEASE, KEY_KEYPAD_0 },
			{ KEY_RELEASE, KEY_KEYPAD_DOT }, key_unknown, key_unknown, key_unknown, { KEY_RELEASE, KEY_F11 }, { KEY_RELEASE, KEY_F12 },
		};
	static const unsigned long scan_codes_limit = sizeof(scan_codes) / sizeof(ScanCode);

	const ScanCode* scan_code(unsigned char code) {
		if (code > scan_codes_limit) return 0;
		return &scan_codes[code];
	}
}