#include "ckeyboard.h"
#include "windowserver/fontchars.h"
#include "windowserver/server/window.h"
#include <syscall.h>
#include <windowserver/client.h>
#include <windowserver/themes/color-active.h>
#include <windowserver/color.h>
#include <windowserver/mainloop.h>
#include <stdio.h>
#include <stdlib.h>
#include <assertions.h>
#include <string.h>

#define INIT_PATH "/usr/bin/"

#define ROWS 20

WindowHandle window;

float textsize = 0.5;

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
int length = 0;
int cmdstart;
int pid = -1;

ElementID labels[ROWS];
ElementID caret;

char path[256];

char history[256][20];
int historylength = 0;
int historyoff = 0;

int rows() {
	int y = 0;
	for (int i = 0; i < length; i++) {
		if (buffer[i] == '\n') y++;
	}
	return y;
}

int caret_x() {
	int x = 0;
	for (int i = 0; i < length; i++) {
		if (buffer[i] == '\n') x = 0;
		else x += (fontchar_for_char(buffer[i]).width * textsize) + 1;
	}
	return x;
}

int caret_y() {
	return rows() * FONTCHAR_41_H * textsize;
}

void update_str() {
	int diff = rows() - ROWS + 1;
	if (diff > 0) {
		int i;
		for (i = 0; i < length; i++) {
			if (buffer[i] == '\n') {
				diff--;
				if (diff != 0) continue;

				i++;
				break;
			}
		}

		memmove(buffer, buffer + i, length - i);
		memset(buffer + length - i, 0, i);
		length -= i;
		cmdstart -= i;
	}
}

void append_strl(const char* string, int len) {
	memcpy(buffer + length, string, len); length += len;
}

void append_str(const char* string) {
	append_strl(string, strlen(string));
}

int write_shell_userpath(char* string) {
	int len = 0;
	int sl;

	memcpy(string + len, "user ", 5); len += 5;

	sl = strlen(path);
	memcpy(string + len, path, sl); len += sl;

	memcpy(string + len, " $ ", 3); len += 3;

	return len;
}

char tmpString[256];
unsigned stdoutfd;
char existsname[32];
char statusname[32];

void execute_command(char* start, int clength) {	
	memset(tmpString, 0, sizeof(tmpString));

	int off = 0;
	if (start[0] != '/') {
		int pl = strlen(path);
		memcpy(tmpString, path, pl);
		off += pl;
	}

	int space0 = clength;
	for (int i = 0; i < clength; i++) {
		if (start[i] == ' ') {
			space0 = i;
			break;
		}
	}

	memcpy(tmpString + off, start, space0);
	off += space0;

	if (strcmp(start, "exit\n") == 0) {
		set_should_close(1);
		return;
	}

	if (strcmp(start, "\n") == 0) {
		return;
	}

	if (strcmp(start, "pwd\n") == 0) {
		memcpy(history[historylength++], "pwd", 4);

		int len = strlen(path);
		memcpy(buffer + length, path, len);
		length += len;
		buffer[length++] = '\n';
		return;
	}

	memcpy(history[historylength++], start, clength);
	historyoff = 0;

	int argc = 0;
	for (int i = space0; i < clength; i++) {
		if (start[i] == ' ') {
			argc++;
		}
	}
	char** argv = 0;
	if (argc != 0) {
		argv = (char**) malloc(sizeof(char*) * argc);
		int lastspace = space0;
		int argid = 0;
		for (int i = space0 + 1; i < clength; i++) {
			if (start[i] == ' ' || i == clength - 1) {
				int len = i == clength - 1? (i - lastspace):(i - lastspace - 1);
				char* arg = malloc(len + 1);
				memcpy(arg, &start[lastspace] + 1, len);
				arg[len] = 0;
				argv[argid++] = arg;
				lastspace = i;
			}
		}
	}

	pid = exec(tmpString, (const char**) argv, argc);
	if (pid == -EFILENOTFOUND) {
		int len = sizeof("File not found '") - 1;
		memcpy(buffer + length, "File not found '", len); length += len;
		memcpy(buffer + length, start, space0); length += space0;
		buffer[length++] = '\'';
		buffer[length++] = '\n';
		return;
	} else if (pid == -EINVALID) {
		int len = sizeof("Not executable '") - 1;
		memcpy(buffer + length, "Not executable '", len); length += len;
		memcpy(buffer + length, start, space0); length += space0;
		buffer[length++] = '\'';
		buffer[length++] = '\n';
		return;
	}

	char stdoutfdname[32];
	memset(stdoutfdname, 0, sizeof(stdoutfdname));
	memset(existsname, 0, sizeof(existsname));
	
	memcpy(existsname, "/proc/", 6);
	itoa(pid, existsname + 6, 10);

	size_t len = strlen(existsname);

	memcpy(stdoutfdname, existsname, sizeof(stdoutfdname));
	memcpy(stdoutfdname + len, "/fd/1", 5);

	stdoutfd = open(stdoutfdname, FILE_FLAG_R | FILE_FLAG_SOCK);

	memcpy(statusname, existsname, sizeof(statusname));
	memcpy(statusname + len, "/status", 7);
}

void render_text() {
	update_str();
	int start = 0;
	int labelID = 0;
	for (int i = 0; i < length; i++) {
		if (buffer[i] == '\n' || i == length - 1) {
			assert(labelID < ROWS);

			memset(tmpString, 0, sizeof(tmpString));
			memcpy(tmpString, buffer + start, i - start + 1);
			labels[labelID] = update_label_detailed(window, labels[labelID], tmpString, 0, 5, (labelID * FONTCHAR_41_H * textsize) + 5, textsize);
			start = i + 1;
			labelID++;
		}
	}
	caret = update_rectangle(window, caret, caret_x() + 5, caret_y() + 5, 8, 19, DEFAULT_TEXT_COLOR);
}

void keydown(KeyEvent* state) {
	if (pid != -1) return;

	if (state->action == KEY_PRESS && state->name == KEY_BACKSPACE) {
		if (length == cmdstart) return;
		buffer[length - 1] = 0;
		length--;
	} else if (state->action == KEY_PRESS && state->name == KEY_UP) {
		historyoff = min(historyoff + 1, historylength);
		memset(buffer + cmdstart, 0, BUFFER_SIZE - cmdstart);
		if (historyoff != 0) {
			int hl = strlen(history[historylength - historyoff]);
			memcpy(buffer + cmdstart, history[historylength - historyoff], hl);
			length = cmdstart + hl;
		} else {
			length = cmdstart;
		}
	} else if (state->action == KEY_PRESS && state->name == KEY_DOWN) {
		historyoff = max(historyoff - 1, 0);
		memset(buffer + cmdstart, 0, BUFFER_SIZE - cmdstart);
		if (historyoff != 0) {
			int hl = strlen(history[historylength - historyoff]);
			memcpy(buffer + cmdstart, history[historylength - historyoff], hl);
			length = cmdstart + hl;
		} else {
			length = cmdstart;
		}
	} else if (state->action == KEY_PRESS && state->name == KEY_C && state->is_ctrl) {
		// set_should_close(1);
	} else if (state->action == KEY_PRESS && state->name == KEY_ENTER) {
		buffer[length++] = '\n';
		execute_command(buffer + cmdstart, length - cmdstart - 1);
		if (pid == -1) {
			length += write_shell_userpath(buffer + length);
			cmdstart = length;
		}
	} else {
		char chr = scan_code_to_char(state);
		if (chr == 0) return;
		buffer[length++] = chr;
	}

	render_text();
	render_window(window);
}

ProcessInfo info;
char stdoutbuffer[1024];
char status[10];
void tick() {
	if (pid != -1) {
		memset(stdoutbuffer, 0, sizeof(stdoutbuffer));
		read(stdoutfd, stdoutbuffer, sizeof(stdoutbuffer));
		size_t len = strlen(stdoutbuffer);
		if (len != 0) {
			memcpy(buffer + length, stdoutbuffer, len); length += len;
		}

		if ((int) open(existsname, FILE_FLAG_R) == -EFILENOTFOUND) {
			length += write_shell_userpath(buffer + length);
			cmdstart = length;

			render_text();
			render_window(window);
			pid = -1;
			return;
		}

		memset(status, 0, sizeof(status));
		read(open(statusname, FILE_FLAG_R | FILE_FLAG_SOCK), status, sizeof(status));
		if (strcmp(status, "running") != 0) {
			length += write_shell_userpath(buffer + length);
			cmdstart = length;

			render_text();
			render_window(window);
			pid = -1;
			return;
		}

		if (len != 0) {
			render_text();
			render_window(window);
		}
	}
}

int main() {
	window = create_window("Shell", 600, (ROWS * FONTCHAR_41_H * textsize) + TITLE_BAR_HEIGHT + 5, 100, 100, 1);

	for (int i = 0; i < ROWS; i++) labels[i] = -1;
	caret = -1;

	memcpy(path, INIT_PATH, sizeof(INIT_PATH) - 1);
	length = write_shell_userpath(buffer);
	cmdstart = length;

	render_text();

	onkeydown(keydown);
	mainloop_cb(window, tick);

	return 0;
}
