#include "syscall.h"
#include <stdio.h>

int main() {
	unsigned pid = exec("sysroot/usr/bin/hello");

	while (is_alive(pid)) yield();
	
	printf("Ended shell\n");
	return 0;
}
