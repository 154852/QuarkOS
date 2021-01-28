#include "kernel/kernel.hpp"
#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/paging.hpp>

void sys_mmap(IRQ::CSITRegisters2* frame) {
#ifdef MMAP_DEBUG
	if (frame->ecx * 4 < 1024) {
		kdebugf("[SysCall::MMAP] Allocating region for process %s: %d pages (%dKB)\n", MultiProcess::get_current_task()->name, frame->ecx, frame->ecx * 4);
	} else if (frame->ecx * 4 < 1024 * 1024) {
		kdebugf("[SysCall::MMAP] Allocating region for process %s: %d pages (%dMB)\n", MultiProcess::get_current_task()->name, frame->ecx, (frame->ecx * 4) / 1024);
	} else {
		kdebugf("[SysCall::MMAP] Allocating region for process %s: %d pages (%dGB)\n", MultiProcess::get_current_task()->name, frame->ecx, (frame->ecx * 4) / (1024 * 1024));
	}
#endif

	MemoryManagement::allocate_region(MultiProcess::get_current_task()->page_dir, frame->ebx, frame->ecx * PAGE_SIZE, false, true, false);
}