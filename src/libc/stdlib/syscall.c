#include <syscall.h>
#include <stdio.h>

void syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3) {
	#ifdef DEBUG_INT
	debugf("[LibC] SYSCALL\n");
	debugf("EAX=%u\nEBX=%u\nECX=%u\nEDX=%u\n\n", type, v1, v2, v3);
	#endif
	asm volatile(
		"int $0x80"
		:: "a"(type), "b"(v1), "c"(v2), "d"(v3)
	);
}

void write(const char* string, unsigned long length) {
	syscall(SC_Write, 1, (unsigned long) string, length);
}

void read(char* string, unsigned long length) {
	syscall(SC_Read, 0, (unsigned long) string, length);
}

void yield() {
	 syscall(SC_Yield, 0, 0, 0);
}

void exit(unsigned char code) {
	 syscall(SC_Exit, code, 0, 0);
	 while (1);
}