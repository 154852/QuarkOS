#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

void sys_yield(IRQ::CSITRegisters2* frame) {
	// MultiProcess::get_current_task()->ring = 3;
	// MultiProcess::yield(frame);
	// MultiProcess::get_current_task()->ring = 0;
}