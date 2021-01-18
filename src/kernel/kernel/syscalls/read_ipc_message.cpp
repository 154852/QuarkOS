#include <kernel/syscall.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/ipcmessaging.hpp>

#include <string.h>
#include <syscall.h>

void sys_read_ipc_message(IRQ::CSITRegisters2 *frame) {
	IPCMessaging::Message msg = IPCMessaging::read_message(MultiProcess::get_current_task()->pid);
	if (!msg.present) {
		frame->eax = -ENOTFOUND;
	} else if (msg.size > frame->ecx) {
		frame->eax = -ETOOSMALL;
	} else {
		memcpy((void*) frame->ebx, msg.raw, msg.size);
		*reinterpret_cast<unsigned*>(frame->edx) = msg.from_pid;
		frame->eax = msg.size;
	}
}