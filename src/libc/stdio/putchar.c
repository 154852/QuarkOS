#include <stdio.h>
#include <syscall.h>
 
void putchar(char ic) {
	char c = (char) ic;
	write(&c, 1);
}