#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

void sys_exit(IRQ::CSITRegisters2* frame) {
	MultiProcess::exit(0, frame->ebx);
	MultiProcess::yield(frame);
}