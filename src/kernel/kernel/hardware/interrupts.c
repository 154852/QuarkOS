#include <stdio.h>
#include <kernel/hardware/interrupts.h>
#include <kernel/hardware/pic.h>

void interrupt_handler(struct interrupt_frame* frame) {
    debugf("Interrupt\n");
    while (1) {}
}

static Descriptor s_idt[256];
static DescriptorTablePointer s_idtr;

void interrupts_initialise(generic_interrupt_handler handlers[256]) {
    for (int i = 0; i < 256; i++) {
        void* ISR = handlers[i]? handlers[i]:interrupt_handler;

        s_idt[i].offset_1 = (u32) ISR & 0xFFFF;
        s_idt[i].selector = 0x0008;
        s_idt[i].zero = 0x00;
        s_idt[i].type_attr = 0b10101110;
        s_idt[i].offset_2 = (u16) ((u32) ISR >> 16);
    }

    s_idtr.address = (u32) s_idt;
    s_idtr.limit = (256 * 8) - 1;
    asm("lidt %0" ::"m"(s_idtr));
}