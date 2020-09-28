#ifndef _SYSCALL_H
#define _SYSCALL_H
 
#include <sys/cdefs.h>
 
#include <stddef.h>
 
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	SC_Write=0x04
} Syscall;
 
void syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3);
void write(const char* string, unsigned long length);
 
#ifdef __cplusplus
}
#endif
 
#endif