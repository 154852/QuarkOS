#include <kernel/interrupts.h>

#ifndef _KERNEL_MULTIPROCESSING_H
#define _KERNEL_MULTIPROCESSING_H

typedef struct {
	// struct interrupt_frame frame;
	unsigned int esp;
} Process;

void init_multiprocessing();
void mp_add_process(unsigned short ip);
void mp_next_context(struct interrupt_frame* out_of_interrupt_frame);

#endif