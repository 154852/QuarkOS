#include "kernel/vga.h"
#include <kernel/hardware/pic.h>
#include <kernel/hardware/pit.h>
#include <kernel/hardware/interrupts.h>
#include <kernel/hardware/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/paging.hpp>
#include <kernel/hardware/gdt.h>
#include <kernel/multiprocess.hpp>
#include <stdio.h>

#include <kernel/tty.h>
#include <stdint2.h>
#include <string.h>

#define CLOCK_SPEED 10

extern "C" u32 clock_exception_state_dump;
asm(
    ".globl clock_exception_state_dump\n"
    "clock_exception_state_dump:\n"
    ".long 0\n"
);

unsigned int count = CLOCK_SPEED;
extern "C" void clock() {
    pic_send_EOI(0);
    
    count--;
    if (!count) {
        count = CLOCK_SPEED;
        switch_process(reinterpret_cast<interrupt_frame*>(clock_exception_state_dump));
    }
}

extern "C" void clock_exception();
asm(
    ".globl clock_exception\n"
    "clock_exception:\n"
        "pusha\n"
        "pushw %ds\n"
        "pushw %es\n"
        "pushw %fs\n"
        "pushw %gs\n"
        "pushw %ss\n"
        "pushw %ss\n"
        "pushw %ss\n"
        "pushw %ss\n"
        "popw %ds\n"
        "popw %es\n"
        "popw %fs\n"
        "popw %gs\n"
        "mov %esp, clock_exception_state_dump\n"
        "call clock\n"
        "popw %gs\n"
        "popw %fs\n"
        "popw %es\n"
        "popw %ds\n"
        "popa\n"
        "iret\n"
);

__attribute__((interrupt)) void page_fault(struct interrupt_frame* frame, unsigned long code) {
    asm("cli");
    printf("Page Fault\n");
    debugf("Page Fault\n");
    while (1) {};
}

__attribute__((interrupt)) void double_fault(struct interrupt_frame* frame, unsigned long code) {
    asm("cli");
    printf("Double Fault\n");
    debugf("Double Fault\n");
    while (1) {};
}

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_OUT_WAIT 10000000

void proc_b() {
    u32 counter = 0;
    while (1) {
        if (counter % TEST_OUT_WAIT == 0) {
            printf("B");
            counter = 0;
        }
        counter++;
    }
}

void proc_a() {
    u32 counter = 0;
    while (1) {
        if (counter % TEST_OUT_WAIT == 0) {
            printf("A");
            counter = 0;
        }
        counter++;
    }
}

#define QUICK_INTERRUPT(name) __attribute__((interrupt)) void name(struct interrupt_frame*, unsigned long) { debugf(#name "\n"); printf(#name "\n"); while (1) {} }

QUICK_INTERRUPT(division_error);
QUICK_INTERRUPT(debug_exception);
QUICK_INTERRUPT(unknown_error);
QUICK_INTERRUPT(breakpoint);
QUICK_INTERRUPT(overflow);
QUICK_INTERRUPT(bounds_check);
QUICK_INTERRUPT(invalid_opcode);
QUICK_INTERRUPT(coprocess_not_available);
QUICK_INTERRUPT(coprocessor_segment_overrun);
QUICK_INTERRUPT(invalid_tss);
QUICK_INTERRUPT(segment_not_present);
QUICK_INTERRUPT(stack_exception);
QUICK_INTERRUPT(general_protection_fault);
QUICK_INTERRUPT(unknonw_error2);
QUICK_INTERRUPT(coprocessor_error);


void* specific_interrupt_handlers[256];
void kernel_main(void) {
    debugf("Starting QuarkOS\n");
    terminal_initialize();

    pic_remap(0x20, 0x28);
    pic_IRQ_clear_mask(2); // Required
    pic_IRQ_clear_mask(1); // Keyboard
    pic_IRQ_clear_mask(0); // Clock

    init_gdt();

    memset(specific_interrupt_handlers, 0, sizeof(specific_interrupt_handlers));
    specific_interrupt_handlers[0x00] = (void*) division_error;
    specific_interrupt_handlers[0x01] = (void*) debug_exception;
    specific_interrupt_handlers[0x02] = (void*) unknown_error;
    specific_interrupt_handlers[0x03] = (void*) breakpoint;
    specific_interrupt_handlers[0x04] = (void*) overflow;
    specific_interrupt_handlers[0x05] = (void*) bounds_check;
    specific_interrupt_handlers[0x06] = (void*) invalid_opcode;
    specific_interrupt_handlers[0x07] = (void*) coprocess_not_available;
    specific_interrupt_handlers[0x08] = (void*) double_fault;
    specific_interrupt_handlers[0x09] = (void*) coprocessor_segment_overrun;
    specific_interrupt_handlers[0x0A] = (void*) invalid_tss;
    specific_interrupt_handlers[0x0B] = (void*) segment_not_present;
    specific_interrupt_handlers[0x0C] = (void*) stack_exception;
    specific_interrupt_handlers[0x0D] = (void*) general_protection_fault;
    specific_interrupt_handlers[0x0E] = (void*) page_fault;
    specific_interrupt_handlers[0x0F] = (void*) unknonw_error2;
    specific_interrupt_handlers[0x10] = (void*) coprocessor_error;

    specific_interrupt_handlers[0x20] = (void*) clock_exception;
    specific_interrupt_handlers[0x21] = (void*) keyboard_interrupt;
    interrupts_initialise((generic_interrupt_handler*) specific_interrupt_handlers);

    pit_set_reload_value(PIT_CHANNEL_0, 100);

    MemoryManagement::init_paging();

    create_kernel_process();

    create_process((void*) proc_a, "Process A");
    create_process((void*) proc_b, "Process B");

    asm("sti");
    for(;;) {
        asm("hlt");
    }
}

#ifdef __cplusplus
}
#endif