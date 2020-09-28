#include <stdio.h>
#include <syscall.h>
 
#if defined(__is_libk)
#include <kernel/tty.hpp>
#endif
 
void putchar(char ic) {
#if defined(__is_libk)
	char c = (char) ic;
	write(&c, 1);
	// terminal_write(&c, sizeof(c));
#else
	// TODO: Implement stdio and the write system call.
#endif
	// return ic;
}