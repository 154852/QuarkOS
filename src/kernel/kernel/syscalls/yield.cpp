#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

void sys_yield(IRQ::CSITRegisters2* frame) {
	MultiProcess::yield(frame);
}