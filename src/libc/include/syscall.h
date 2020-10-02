#ifndef _SYSCALL_H
#define _SYSCALL_H
 
#include <sys/cdefs.h>
 
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SC_Write=0x04,
	SC_Yield=0x17,
	SC_Exit=0x37
} Syscall;
 
void syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3);
void write(const char* string, unsigned long length);
void yield();
void exit(unsigned char code);
 
#ifdef __cplusplus
}
#endif
 
#endif