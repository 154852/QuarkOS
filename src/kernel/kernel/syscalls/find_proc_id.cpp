#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

#include <string.h>
#include <syscall.h>

void sys_find_proc_by_id(IRQ::CSITRegisters2 *frame) {
	const char* name = reinterpret_cast<const char*>(frame->ebx);
	if (strcmp(MultiProcess::get_current_task()->name, name) == 0) {
		frame->eax = MultiProcess::get_current_task()->pid;
		return;
	}

	MultiProcess::Process* proc = MultiProcess::get_current_task()->next;
	while (proc != MultiProcess::get_current_task()) {
		if (strcmp(proc->name, name) == 0) {
			frame->eax = proc->pid;
			return;
		}
		proc = proc->next;
	}
	frame->eax = -ENOTFOUND;
}