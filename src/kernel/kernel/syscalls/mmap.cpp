#include "kernel/kernel.hpp"
#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/paging.hpp>

void sys_mmap(IRQ::CSITRegisters2* frame) {
#ifdef MMAP_DEBUG
	char string[16];
	MemoryManagement::mem_size_to_str(string, frame->ecx * PAGE_SIZE);
	kdebugf("[SysCall::MMAP] Allocating region for process %s: %d pages (%s)\n", MultiProcess::get_current_task()->name, frame->ecx, string);
#endif

	MemoryManagement::allocate_region(MultiProcess::get_current_task()->page_dir, frame->ebx, frame->ecx * PAGE_SIZE, false, true, false);
}