#include <stdint2.h>

#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

struct interrupt_frame {
    u16 ip;
    u16 cs;
    u16 flags;
    u16 sp;
    u16 ss;
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