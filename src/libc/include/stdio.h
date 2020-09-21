#ifndef _STDIO_H
#define _STDIO_H 1
 
#include <sys/cdefs.h>
 
#define EOF (-1)
 
#ifdef __cplusplus
extern "C" {
#endif
 
int printf(const char* __restrict, ...);
int debugf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char value);
void io_wait();
 
#ifdef __cplusplus
}
#endif
 
#endif