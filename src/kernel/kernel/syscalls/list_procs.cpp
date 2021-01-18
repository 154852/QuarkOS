#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>

void sys_list_procs(IRQ::CSITRegisters2 *frame) {
	unsigned int length = 0;
	unsigned int* pids = reinterpret_cast<unsigned int*>(frame->ebx);
	unsigned long max_length = frame->ecx;

	if (max_length == 0) {
		frame->eax = 0;
		return;
	}

	pids[0] = MultiProcess::get_current_task()->pid;
	length++;

	MultiProcess::Process* proc = MultiProcess::get_current_task()->next;
	while (proc != MultiProcess::get_current_task()) {
		if (length >= max_length) {
			frame->eax = length;
			return;
		}
		
		pids[length++] = proc->pid;
		proc = proc->next;
	}
	frame->eax = length;
}