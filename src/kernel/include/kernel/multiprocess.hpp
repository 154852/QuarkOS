#include <stdint2.h>
#include <kernel/paging.hpp>
#include <kernel/hardware/interrupts.hpp>

#ifndef _KERNEL_MULTIPROCESS_H
#define _KERNEL_MULTIPROCESS_H

struct ThreadControlBlock {
    IRQ::InterruptFrame frame;
    const char* name;
    bool kernel;
    ThreadControlBlock* next;
};

void switch_process(IRQ::InterruptFrame* frame);
ThreadControlBlock* create_process(void* entry, const char* name);
ThreadControlBlock* create_kernel_process();

#endif