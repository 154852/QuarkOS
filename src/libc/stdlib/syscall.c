#include <syscall.h>

void syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3) {
	asm volatile(
		"movl %0, %%eax\n"
		"movl %1, %%ebx\n"
		"movl %2, %%ecx\n"
		"movl %3, %%edx\n"
		"int $0x80"
		:: "r"(type), "r"(v1), "r"(v2), "r"(v3)
	);
}

void write(const char* string, unsigned long length) {
	syscall(SC_Write, 1, (unsigned long) string, length);
}