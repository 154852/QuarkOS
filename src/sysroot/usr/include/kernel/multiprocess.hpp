#include <stdint2.h>
#include <kernel/paging.hpp>
#include <kernel/hardware/interrupts.hpp>

#ifndef _KERNEL_MULTIPROCESS_H
#define _KERNEL_MULTIPROCESS_H

namespace MultiProcess {
    enum ProcessState {
        Running,
        Runnable,
        Exitting
    };

    struct Process {
        IRQ::CSITRegisters registers;
        Process* next;
        const char* name;
        ProcessState state;
        MemoryManagement::PageDirectory* page_dir;
    };

    struct __attribute__((packed)) TSS {
        u32 prevTss;
        u32 esp0;
        u32 ss0;
        u32 esp1;
        u32 ss1;
        u32 esp2;
        u32 ss2;
        u32 cr3;
        u32 eip;
        u32 eflags;
        u32 eax;
        u32 ecx;
        u32 edx;
        u32 ebx;
        u32 esp;
        u32 ebp;
        u32 esi;
        u32 edi;
        u32 es;
        u32 cs;
        u32 ss;
        u32 ds;
        u32 fs;
        u32 gs;
        u32 ldt;
        u16 trap;
        u16 iomap;
    };

    extern "C" void context_switch_interrupt_trigger();
    void tss_set_stack(u32 kss, u32 kesp);

    Process* create(void* entry, const char* name);
    void end();
    void append(Process* process);
    void init(u32 ktss_idx, u32 kss, u32 kesp);
    void exit(Process* process, u32 exit_code);
    void yield(IRQ::CSITRegisters2* regs);
};

#endif