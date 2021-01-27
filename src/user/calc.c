#include "windowserver/server/window.h"
#include "windowserver/themes/color-active.h"
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <stdio.h>
#include <assertions.h>
#include <windowserver/wsmsg.h>
#include <windowserver/client.h>
#include <windowserver/mainloop.h>
#include <ckeyboard.h>

WindowHandle windowhandle;
ElementID textField;
#define TEXT_SIZE 32
char text[TEXT_SIZE] = "0.0";
int textIdx = 0;
char textneedsclear = 1;
const int gridsize = 50;
char hasans;
float ans;

typedef struct {
	ElementID button;
	ElementID label;

	const char* text;
	
	int x;
	int y;

	int xoff;

	char isspecial;
} CalcButton;

#define GRID_WIDTH 4
#define GRID_HEIGHT 5

CalcButton buttons[] = {
	(CalcButton) {
		.text="ANS",
		.x=0, .y=0,
		.isspecial=1,
		.xoff=-9
	},
	(CalcButton) {
		.text="",
		.x=1, .y=0
	},
	(CalcButton) {
		.text="AC",
		.x=2, .y=0,
		.isspecial=1,
		.xoff=-5
	},
	(CalcButton) {
		.text="DEL",
		.x=3, .y=0,
		.isspecial=1,
		.xoff=-9
	},
	
	(CalcButton) {
		.text="7",
		.x=0, .y=1
	},
	(CalcButton) {
		.text="8",
		.x=1, .y=1
	},
	(CalcButton) {
		.text="9",
		.x=2, .y=1
	},
	(CalcButton) {
		.text="*",
		.x=3, .y=1,
		.isspecial=1
	},

	(CalcButton) {
		.text="4",
		.x=0, .y=2
	},
	(CalcButton) {
		.text="5",
		.x=1, .y=2
	},
	(CalcButton) {
		.text="6",
		.x=2, .y=2
	},
	(CalcButton) {
		.text="-",
		.x=3, .y=2,
		.isspecial=1
	},

	(CalcButton) {
		.text="1",
		.x=0, .y=3
	},
	(CalcButton) {
		.text="2",
		.x=1, .y=3
	},
	(CalcButton) {
		.text="3",
		.x=2, .y=3
	},
	(CalcButton) {
		.text="+",
		.x=3, .y=3,
		.isspecial=1
	},

	(CalcButton) {
		.text=".",
		.x=0, .y=4,
		.isspecial=0
	},
	(CalcButton) {
		.text="0",
		.x=1, .y=4
	},
	(CalcButton) {
		.text="=",
		.x=2, .y=4,
		.isspecial=1
	},
	(CalcButton) {
		.text="/",
		.x=3, .y=4,
		.isspecial=1
	},
};
const int buttonCount = sizeof(buttons) / sizeof(CalcButton);

char is_num(const char chr) {
	return chr >= '0' && chr <= '9';
}

#define EVAL_ERR_SYNTAX 1
#define EVAL_ERR_MATH 2

float evaluate(const char* string, char* error) {
	const char* end = string + strlen(string);
	if (end <= string) {
		*error = EVAL_ERR_SYNTAX;
		return 0;
	}
	while (string[0] == ' ') {
		string++;
	}

	float left;
	if (memcmp(string, "ANS", 3) == 0) {
		left = ans;
		string += 3;
	} else if (is_num(string[0])) {
		float integer = 0;
		while (is_num(string[0])) {
			integer *= 10;
			integer += string[0] - '0';
			string++;
		}

		if (string[0] == '.') {
			float fraction = 0;
			float next = 0.1;
			string++;

			while (is_num(string[0])) {
				fraction += next * (string[0] - '0');
				next *= 0.1;
				string++;
			}

			integer += fraction;
		}

		left = integer;
	} else {
		*error = EVAL_ERR_SYNTAX;
		return 0;
	}

	while (string[0] == ' ') {
		string++;
	}

	if (string >= end) return left;

	char operator;
	operator = string[0];
	string += 2; // '+ '

	float right = evaluate(string, error);
	if (*error) return 0;
	
	switch (operator) {
		case '+': return left + right;
		case '*': return left * right;
		case '/': {
			if (right == 0) {
				*error = EVAL_ERR_MATH;
				return 0;
			}
			return left / right;
		}
		case '-': return left - right;
		default: {
			*error = EVAL_ERR_SYNTAX;
			return 0;
		}
	}

	return 0.0;
}

void str_putchar(char chr) {
	text[textIdx++] = chr;
}

void oncalcbuttonclick(int buttonIdx) {
	CalcButton* btn = &buttons[buttonIdx];

	if (btn->text[0] == 0) return;

	if (textneedsclear) {
		memset(text, 0, TEXT_SIZE);
		textIdx = 0;
		textneedsclear = 0;
	}

	if (strcmp(btn->text, "DEL") == 0) {
		if (textIdx != 0) text[--textIdx] = 0;
	} else if (strcmp(btn->text, "ANS") == 0) {
		if (hasans) {
			memcpy(text + textIdx, "ANS", sizeof("ANS") - 1);
			textIdx += 3;
		} else {
			return;
		}
	} else if (strcmp(btn->text, "AC") == 0) {
		memset(text, 0, TEXT_SIZE);
		textIdx = 0;
		memcpy(text, "0.0", 3);
		textneedsclear = 1;
	} else if (btn->text[0] == '=') {
		char err;
		float value = evaluate(text, &err);
		memset(text, 0, TEXT_SIZE);
		textIdx = 0;
		textneedsclear = 1;

		if (err == EVAL_ERR_SYNTAX) {
			memcpy(text, "Syntax Error", sizeof("Syntax Error"));
		} else if (err == EVAL_ERR_MATH) {
			memcpy(text, "Math Error", sizeof("Math Error"));
		} else {
			printf_(str_putchar, "= %f", value);
			ans = value;
			hasans = 1;
		}
	} else {
		if (btn->isspecial) {
			if (textIdx == 0 && hasans) {
				memcpy(text, "ANS", sizeof("ANS") - 1);
				textIdx += 3;
			}

			text[textIdx++] = ' ';
		}
		text[textIdx++] = btn->text[0];
		if (btn->isspecial) text[textIdx++] = ' ';
	}

	update_label(windowhandle, textField, text, 0, (gridsize * 0.2), (gridsize * 0.1));
}

#define TEXT_FIELD_HEIGHT 22

int main() {
	windowhandle = create_window("Calculator", (gridsize * GRID_WIDTH) + (gridsize * 0.1), (gridsize * GRID_HEIGHT) + TITLE_BAR_HEIGHT + (gridsize * 0.1 * 2) + TEXT_FIELD_HEIGHT, 100, 100, 1);

	textField = create_label(windowhandle, text, 0, (gridsize * 0.2), (gridsize * 0.1));

	for (int i = 0; i < buttonCount; i++) {
		int x = (gridsize * buttons[i].x) + (gridsize * 0.1);
		int y = (gridsize * buttons[i].y) + (gridsize * 0.1 * 2) + TEXT_FIELD_HEIGHT;
		buttons[i].button = create_button(windowhandle, x, y, gridsize * 0.9, gridsize * 0.9, &COLOR_SECONDARY_BACKGROUND);
		int tx = buttons[i].xoff + (gridsize * 0.35);
		buttons[i].label = create_label(windowhandle, buttons[i].text, 0, x + tx, y + (gridsize * 0.25));
		onclick(buttons[i].button, i, oncalcbuttonclick);
	}

	
	mainloop(windowhandle);

	return 0;
}
