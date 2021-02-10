#ifndef _STDIO_H
#define _STDIO_H 1
 
#include <sys/cdefs.h>
 
#define EOF (-1)
 
#ifdef __cplusplus
extern "C" {
#endif

int printf_(void(*_putchar)(char character), const char* format, ...);

void putchar(char ch);
void debug_putchar(char ch);

#ifdef __is_kernel
void terminal_putchar(char ch);
#define tprintf(fmt, args...) printf_(terminal_putchar, fmt, ##args)
#endif

#define printf(fmt, args...) printf_(putchar, fmt, ##args)
#define debugf(fmt, args...) printf_(debug_putchar, fmt, ##args)

void puts(const char*);

unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char value);
unsigned short inw(unsigned short port);
void outw(unsigned short port, unsigned short value);
void inw_many(unsigned short port, unsigned char* buffer, unsigned length);
void outw_many(unsigned short port, unsigned char* buffer, unsigned length);
unsigned int indw(unsigned short port);
void outdw(unsigned short port, unsigned int value);
void io_wait();
 
#ifdef __cplusplus
}
#endif
 
#endif