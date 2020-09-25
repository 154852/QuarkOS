#include <stdint2.h>

#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

struct interrupt_frame {
    u16 gs;
    u16 fs;
    u16 es;
    u16 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 eip;
    u16 cs;
    u16 __csPadding;
    u32 eflags;
    u32 esp_if_crossRing;
    u16 ss_if_crossRing;
};

typedef struct  __attribute__ ((__packed__)) {
   uint16_t offset_1;
   uint16_t selector;
   uint8_t zero;
   uint8_t type_attr;
   uint16_t offset_2;
} Descriptor;

typedef void(*generic_interrupt_handler)(struct interrupt_frame* frame);
__attribute__((interrupt)) void interrupt_handler(struct interrupt_frame* frame);

typedef struct __attribute__ ((__packed__)) {
    u16 limit;
    u32 address;
} DescriptorTablePointer;

void interrupts_initialise(generic_interrupt_handler handlers[256]);

#endif