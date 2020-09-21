#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/interrupts.h>
#include <kernel/keyboard.h>
#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/gdt.h>
// #include <kernel/multiprocessing.h>
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

void* specific_interrupt_handlers[256];
void kernel_main(void) {
    terminal_initialize();

    pic_remap(0x20, 0x28);
    pic_IRQ_clear_mask(2); // Required
    pic_IRQ_clear_mask(1); // Keyboard
    // pic_IRQ_clear_mask(0); // Clock

    init_gdt();

    memset(specific_interrupt_handlers, 0, sizeof(specific_interrupt_handlers));
    specific_interrupt_handlers[14] = page_fault;
    specific_interrupt_handlers[0x20] = clock;
    specific_interrupt_handlers[0x21] = keyboard_interrupt;
    interrupts_initialise((generic_interrupt_handler*) specific_interrupt_handlers);

    pit_set_reload_value(PIT_CHANNEL_0, 100);

    init_paging();

    // u32 value = *((u32*) 0);
    // debugf("Val: %i\n", value);
    
    asm("sti");
    for(;;) {
        asm("hlt");
    }
}