// NOTE: This will not currently work, it was made for the old terminal interface

#include "syscall.h"
#include <stdio.h>

int main() {
	debugf("What is your name? ");

	char name[10];
	for (int i = 0; i < 9; i++) {
		char next;
		read(FD_STDIN, &next, 1);
		if (next == '\n') break;
		printf("%c", next);
		name[i] = next;
	}
	debugf("\n");

	debugf("Hello %s!\n", name);
	return 0;
}
