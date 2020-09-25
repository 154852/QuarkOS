#include <string.h>
#include <kernel/kmalloc.hpp>
#include <kernel/multiprocess.hpp>

static ThreadControlBlock* current_process = 0;

void switch_process(interrupt_frame* frame) {
    memcpy(&current_process->frame, frame, sizeof(interrupt_frame));
    do {
        current_process = current_process->next;
    } while (current_process->kernel);
    memcpy(frame, &current_process->frame, sizeof(interrupt_frame));
}

ThreadControlBlock* create_process(void* entry, const char* name) {
    ThreadControlBlock* block = (ThreadControlBlock*) kmalloc(sizeof(ThreadControlBlock), 0, 0);
    block->name = name;
    block->frame.ebp = kmalloc(0x1000, 0, 0);
    block->frame.esp = block->frame.ebp;
    block->frame.cs = 0x08;
    block->frame.ds = 0x10;
    block->frame.ss_if_crossRing = 0x10;
    block->frame.eip = (u32) entry;
    block->frame.eflags |= 0x0200; // enable interrupts

    block->next = current_process->next;
    current_process->next = block;

    return block;
}

ThreadControlBlock* create_kernel_process() {
    ThreadControlBlock* block = (ThreadControlBlock*) kmalloc(sizeof(ThreadControlBlock), 0, 0);
    block->name = "kernel";
    block->kernel = true;
    block->next = block;

    current_process = block;

    return block;
}