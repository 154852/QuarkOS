#include "syscall.h"
#include <stdio.h>

int main() {
	printf("What is your name? ");
	// yield();

	char name[10];
	for (int i = 0; i < 9; i++) {
		char next;
		read(FD_STDIN, &next, 1);
		if (next == '\n') break;
		printf("%c", next);
		name[i] = next;
	}
	printf("\n");

	printf("Hello %s!\n", name);
	return 0;
}
