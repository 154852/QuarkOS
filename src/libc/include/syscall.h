#ifndef _SYSCALL_H
#define _SYSCALL_H
 
#include <sys/cdefs.h>
 
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SC_Read=0x00,
	// SC_Write=0x01,
	// SC_Yield=0x18,
	// SC_Exit=0x3c
	SC_Write=0x04,
	SC_Yield=0x17,
	SC_Exit=0x37
} Syscall;
 
void syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3);
void write(const char* string, unsigned long length);
void read(char* string, unsigned long length);
void yield();
void __attribute__((noreturn)) exit(unsigned char code);
 
#ifdef __cplusplus
}
#endif
 
#endif