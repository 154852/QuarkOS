#include <stdio.h>
#include <syscall.h>
 
void putchar(char ic) {
	write(FD_STDOUT, &ic, 1);
}