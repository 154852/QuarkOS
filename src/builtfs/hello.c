#include "syscall.h"
#include <stdio.h>

int main() {
	printf("Hello World!\n");

	char name[10];
	for (int i = 0; i < 9; i++) {
		debugf("I=%d\n", i);
		char next;
		read(&next, 1);
		if (next == '\n') break;
		name[i] = next;
	}

	printf("Hello %s!\n", name);
	return 42;
}
