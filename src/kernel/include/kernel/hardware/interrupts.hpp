#include <stdint2.h>

#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

namespace IRQ {
    struct InterruptFrame {
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

    struct __attribute__ ((__packed__)) Descriptor {
        uint16_t offset_1;
        uint16_t selector;
        uint8_t zero;
        uint8_t type_attr;
        uint16_t offset_2;
    };

    typedef void(*GenericInterruptHandler)(InterruptFrame* frame);
    __attribute__((interrupt)) void interrupt_handler(InterruptFrame* frame);

    struct __attribute__ ((__packed__)) DescriptorTablePointer {
        u16 limit;
        u32 address;
    };

    void interrupts_initialise(GenericInterruptHandler handlers[256]);

    inline void enable_irq() {
        asm("sti");
    }

    inline void disable_irq() {
        asm("cli");
    }
}

#endif