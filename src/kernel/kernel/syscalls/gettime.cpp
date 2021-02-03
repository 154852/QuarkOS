#include "kernel/kernel.hpp"
#include <kernel/syscall.hpp>
#include <kernel/hardware/pit.hpp>

void sys_gettime(IRQ::CSITRegisters2* frame) {
	*reinterpret_cast<unsigned long long*>(frame->ebx) = PIT::get_time();
}