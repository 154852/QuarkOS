// NOTE: This will not currently work, it was made for the old terminal interface

#include "syscall.h"
#include <stdio.h>
#include <assertions.h>
#include <string.h>

void read_input(char* value, int length, int* str_length) {
	int i;
	for (i = 0; i < length - 1; i++) {
		char next;
		read(FD_STDIN, &next, 1);
		if (next == '\n') break;
		if (next == 0x08) {
			i -= 2;
			continue;
		}

		printf("%c", next);
		value[i] = next;
	}
	printf("\n");

	*str_length = i;
	memset(value + i, 0, length - i);
}

void concat(char* a, int a_size, char* b, int b_size, char* dest, int dest_size) {
	assert(a_size + b_size <= dest_size);

	memcpy(dest, a, a_size);
	memcpy(dest + a_size, b, b_size);
	memset(dest + a_size + b_size, 0, dest_size - (a_size + b_size));
}

#define PWD_SIZE 100
#define INPUT_SIZE 100

#define INIT_PATH "sysroot/usr/bin/"

int main() {
	int pwd_length = sizeof(INIT_PATH) - 1;
	char pwd[PWD_SIZE];
	memcpy(pwd, INIT_PATH, sizeof(INIT_PATH));

	printf("Session started at %s\n", pwd);

	int input_length;
	char input[INPUT_SIZE];

	char tmp_path[PWD_SIZE];

	// ProcessInfo info;

	while (1) {
		printf("$ ");
		read_input(input, INPUT_SIZE, &input_length);

		if (memcmp(input, "exit", 5) == 0) break;
		
		if (memcmp(input, "pwd", 4) == 0) {
			printf("%s\n", pwd);
			continue;
		}

		concat(pwd, pwd_length, input, input_length, tmp_path, PWD_SIZE);
		unsigned pid = exec(tmp_path);

		if ((int) pid == -EFILENOTFOUND) {
			printf("File not found %s\n", tmp_path);
			continue;
		}

		// info.pid = pid;
		// do {
		// 	proc_info(&info);
		// } while (info.state == PSSC_Running);
		// printf("%s ended\n", tmp_path);
	}

	printf("Session ended\n", tmp_path);
	
	return 0;
}
