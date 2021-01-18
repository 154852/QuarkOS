#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

#include <syscall.h>
#include <string.h>

void sys_proc_info(IRQ::CSITRegisters2* frame) {
	ProcessInfo* info = reinterpret_cast<ProcessInfo*>(frame->ebx);

	MultiProcess::Process* proc = MultiProcess::find_process_by_pid(info->pid);
	if (proc == 0) {
		info->state = ProcessStateSC::PSSC_NotPresent;
	} else {
		if (proc->state == MultiProcess::Exitting) info->state = ProcessStateSC::PSSC_Exitting;
		else if (proc->state == MultiProcess::Idle) info->state = ProcessStateSC::PSSC_Idle;
		else info->state = ProcessStateSC::PSSC_Running;

		memcpy(info->name, proc->name, strlen(proc->name) + 1);
	}
}