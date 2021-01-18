#include <kernel/syscall.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/ipcmessaging.hpp>

#include <string.h>

void sys_send_ipc_message(IRQ::CSITRegisters2 *frame) {
	void* copied_raw = kmalloc(frame->edx);
	memcpy(copied_raw, (void*) frame->ecx, frame->edx);
	IPCMessaging::send_message(MultiProcess::get_current_task()->pid, frame->ebx, copied_raw, frame->edx);
}