#include <stdint2.h>
#include <kernel/paging.hpp>
#include <kernel/hardware/interrupts.hpp>
#include <kernel/socket.hpp>

#ifndef _KERNEL_MULTIPROCESS_H
#define _KERNEL_MULTIPROCESS_H

namespace MultiProcess {
    /*
Processes have states, a process running in it's normal state, will be switching between Running, and Runnable.
A process is Running if it is the process running right now.
A process is Runnable if it could be the process running, say on the next rotation.
A process is Exitting if it should be opened again, and will be removed on the next cycle.
A process is Idle if it should be given no computation time unless there is no other process running. This should only be used for the idle process.

Asyncronous syscall actions (such as reading a file), are handled by wait task callbacks. If the process is Waiting, that means that the processes thread execution time is being used by the wait task.
Wait tasks run in the kernel, outside of an interrupt, but are still ring 3. A process should mark itself as BeginWaiting, when it wants to start a wait process, then it will be automatically switched to Waiting once the wait process ahas started, 
then once the wait process is finished, it should be switched to EndWaiting.

Syncronous syscall actions (such as switching page directory), are handled bu ring0task callbacks. This should be marked by setting process.ring0_request.has_ring0_request = true.
Ring0SyncRequests is run as a callback before the next time slice of the process. It is run in the kernel, inside an interrupt, at ring 0.
    */

    enum ProcessState {
        Running,
        Runnable,
        Exitting,
        Idle,
    };

    struct Process;

    struct Process {
        u32 pid;
        IRQ::CSITRegisters registers;
        Process* next;
        const char* name;
        ProcessState state;
        bool is_kernel;
        MemoryManagement::PageDirectory* page_dir;

        void* kernel_esp;

        Socket::Socket* fd0; // stdin
        Socket::Socket* fd1; // stdout
        Socket::Socket* fd2; // stderr
        Socket::Socket* status_sock; // status

        bool definitely_has_noipcmsg;

        u32 esp;
        u32 ebp;
        u32 eip;

        bool spawned;
        
        char ring;
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
    Process* get_current_task();
    void append(Process* process);
    void init(u32 ktss_idx, u32 kss, u32 kesp);
    void exit(Process* process, u32 exit_code);
    void yield(IRQ::CSITRegisters2* regs);
    Process* find_process_by_pid(u32 pid);
};

#endif