#include <stdio.h>
#include <syscall.h>
 
void putchar(char ic) {
	char c = (char) ic;
	write(FD_STDOUT, &c, 1);
}