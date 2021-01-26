#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

#include <kernel/hardware/interrupts.hpp>

void sys_read(IRQ::CSITRegisters2* frame);
void sys_write(IRQ::CSITRegisters2* frame);
void sys_open(IRQ::CSITRegisters2* frame);
void sys_exit(IRQ::CSITRegisters2* frame);
void sys_yield(IRQ::CSITRegisters2* frame);
void sys_exec(IRQ::CSITRegisters2* frame);
void sys_framebuffer_info(IRQ::CSITRegisters2* frame);
void sys_framebuffer_set_state(IRQ::CSITRegisters2* frame);
void sys_proc_info(IRQ::CSITRegisters2* frame);
void sys_get_pid(IRQ::CSITRegisters2* frame);
void sys_list_procs(IRQ::CSITRegisters2* frame);
void sys_send_ipc_message(IRQ::CSITRegisters2* frame);
void sys_read_ipc_message(IRQ::CSITRegisters2* frame);
void sys_read_ipc_message(IRQ::CSITRegisters2* frame);
void sys_find_proc_by_id(IRQ::CSITRegisters2* frame);
void sys_mmap(IRQ::CSITRegisters2* frame);

#endif