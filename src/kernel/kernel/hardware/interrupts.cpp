#include <stdio.h>
#include <kernel/hardware/interrupts.hpp>
#include <kernel/hardware/pic.hpp>

void IRQ::interrupt_handler(IRQ::CSITRegisters*) {
    debugf("Interrupt\n");
    while (1) {}
}

static IRQ::Descriptor s_idt[256];
static IRQ::DescriptorTablePointer s_idtr;

void IRQ::interrupts_initialise(IRQ::GenericInterruptHandler handlers[256]) {
    for (int i = 0; i < 256; i++) {
        u32 ISR = (u32) (handlers[i]? handlers[i]:IRQ::interrupt_handler);

        s_idt[i].offset_1 = (u32) ISR & 0xFFFF;
        s_idt[i].selector = 0x0008;
        s_idt[i].zero = 0x00;
        s_idt[i].type = 0b1110;
        s_idt[i].storage_segment = 0;
        s_idt[i].dpl = 0b11;
        s_idt[i].present = 1;
        s_idt[i].offset_2 = (u16) ((u32) ISR >> 16);
    }

    s_idtr.address = (u32) s_idt;
    s_idtr.limit = (256 * 8) - 1;
    asm("lidt %0" ::"m"(s_idtr));
}