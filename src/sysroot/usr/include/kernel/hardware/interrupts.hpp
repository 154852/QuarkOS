#include <stdint2.h>

#ifndef _KERNEL_INTERRUPTS_H
#define _KERNEL_INTERRUPTS_H

namespace IRQ {
    struct __attribute__((packed)) CSITRegisters {
        u32 ebp;
        u32 edi;
        u32 esi;
        u32 edx;
        u32 ecx;
        u32 ebx;
        u32 eax;

        u32 eip;
        u32 cs;
        u32 eflags;
        u32 esp;
        u32 ss;
    };

    struct __attribute__((packed)) CSITRegisters2 {
        u32 eax_ds;

        u32 ebp;
        u32 edi;
        u32 esi;
        u32 edx;
        u32 ecx;
        u32 ebx;
        u32 eax;

        u32 eip;
        u32 cs;
        u32 eflags;
        u32 esp;
        u32 ss;
    };
    
    struct __attribute__ ((__packed__)) Descriptor {
        u64 offset_1 : 16;
        u64 selector : 16;
        u64 zero : 8;
        u64 type : 4;
        u64 storage_segment : 1;
        u64 dpl : 2;
        u64 present : 1;
        u16 offset_2;
    };

    typedef void(*GenericInterruptHandler)(CSITRegisters* frame);
    __attribute__((interrupt)) void interrupt_handler(CSITRegisters* frame);

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