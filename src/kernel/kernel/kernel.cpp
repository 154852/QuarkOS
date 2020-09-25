#include <kernel/vga.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/hardware/pit.hpp>
#include <kernel/hardware/interrupts.hpp>
#include <kernel/hardware/keyboard.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/paging.hpp>
#include <kernel/hardware/gdt.hpp>
#include <kernel/multiprocess.hpp>
#include <stdio.h>
#include <kernel/tty.hpp>
#include <stdint2.h>
#include <string.h>

#define CLOCK_SPEED 1

extern "C" u32 clock_exception_state_dump;
asm(
    ".globl clock_exception_state_dump\n"
    "clock_exception_state_dump:\n"
    ".long 0\n"
);

unsigned int count = CLOCK_SPEED;
extern "C" void clock() {
    PIC::send_EOI(0);
    
    count--;
    if (!count) {
        count = CLOCK_SPEED;
        switch_process(reinterpret_cast<IRQ::InterruptFrame*>(clock_exception_state_dump));
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

__attribute__((interrupt)) void page_fault(struct interrupt_frame*, unsigned long) {
    asm("cli");
    printf("Page Fault\n");
    debugf("Page Fault\n");
    while (1) {};
}

__attribute__((interrupt)) void double_fault(struct interrupt_frame*, unsigned long) {
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
    char B = 'B';
    while (1) {
        if (counter % TEST_OUT_WAIT == 0) {
            terminal_reset();
            terminal_write(&B, 1);
            counter = 0;
        }
        counter++;
    }
}

void proc_a() {
    u32 counter = 0;
    char A = 'A';
    while (1) {
        if (counter % TEST_OUT_WAIT == 0) {
            terminal_reset();
            terminal_write(&A, 1);
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
extern "C" void kernel_main(void) {
    debugf("Starting QuarkOS\n");
    terminal_initialize();

    PIC::remap(0x20, 0x28);
    PIC::irq_clear_mask(2); // Required
    PIC::irq_clear_mask(1); // Keyboard
    PIC::irq_clear_mask(0); // Clock

    GDT::initialise();

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
    specific_interrupt_handlers[0x21] = (void*) Keyboard::keyboard_interrupt;
    IRQ::interrupts_initialise((IRQ::GenericInterruptHandler*) specific_interrupt_handlers);

    PIT::set_reload_value(PIT_CHANNEL_0, 100);

    MemoryManagement::init_paging();

    create_kernel_process();

    create_process((void*) proc_a, "Process A");
    create_process((void*) proc_b, "Process B");

    asm("sti");
    for(;;) { asm("hlt"); }
}

#ifdef __cplusplus
}
#endif