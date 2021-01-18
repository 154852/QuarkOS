#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

void sys_get_pid(IRQ::CSITRegisters2* frame) {
	frame->eax = MultiProcess::get_current_task()->pid;
}