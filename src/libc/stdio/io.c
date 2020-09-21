#include <stdio.h>

unsigned char inb(unsigned short port) {
    char value;
    asm volatile("inb %1, %0"
                 : "=a"(value)
                 : "Nd"(port));
    return value;
}

void outb(unsigned short port, unsigned char value) {
    asm volatile("outb %0, %1" ::"a"(value), "Nd"(port));
}

void io_wait() {
    asm volatile("outb %%al, $0x80" : : "a"(0));
}