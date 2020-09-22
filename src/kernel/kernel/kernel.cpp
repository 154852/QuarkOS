#include <kernel/hardware/pic.h>
#include <kernel/hardware/pit.h>
#include <kernel/hardware/interrupts.h>
#include <kernel/hardware/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/paging.hpp>
#include <kernel/hardware/gdt.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <stdint2.h>
#include <string.h>

#define CLOCK_SPEED 10

unsigned int count = CLOCK_SPEED;
__attribute__((interrupt)) void clock(struct interrupt_frame* frame) {
    pic_send_EOI(0);
    
    count--;
    if (!count) {
        count = CLOCK_SPEED;
    }
}

__attribute__((interrupt)) void page_fault(struct interrupt_frame* frame, unsigned long code) {
    asm("cli");
    printf("Page Fault\n");
    debugf("Page Fault\n");
    while (1) {};
}

#ifdef __cplusplus
extern "C" {
#endif

void* specific_interrupt_handlers[256];
void kernel_main(void) {
    terminal_initialize();

    pic_remap(0x20, 0x28);
    pic_IRQ_clear_mask(2); // Required
    pic_IRQ_clear_mask(1); // Keyboard
    // pic_IRQ_clear_mask(0); // Clock

    init_gdt();

    memset(specific_interrupt_handlers, 0, sizeof(specific_interrupt_handlers));
    specific_interrupt_handlers[14] = (void*) page_fault;
    specific_interrupt_handlers[0x20] = (void*) clock;
    specific_interrupt_handlers[0x21] = (void*) keyboard_interrupt;
    interrupts_initialise((generic_interrupt_handler*) specific_interrupt_handlers);

    pit_set_reload_value(PIT_CHANNEL_0, 100);

    MemoryManagement::init_paging();

    asm("sti");
    for(;;) {
        asm("hlt");
    }
}

#ifdef __cplusplus
}
#endif