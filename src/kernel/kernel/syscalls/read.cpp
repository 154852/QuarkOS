#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/ustar.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/keyboard.hpp>

#include <assertions.h>
#include <syscall.h>
#include <string.h>

void read_syscall_wait_task() {
    MultiProcess::Process* task = MultiProcess::get_current_task();
    
	unsigned id = task->registers.ebx >> 8;
	USTAR::FileParsed* file = USTAR::lookup_parsed_from_raw_pointer(id);
	unsigned size = task->registers.edx > file->length? file->length:task->registers.edx;
	memcpy((void*) task->registers.ecx, file->content, size);
	task->registers.eax = size;

    task->state = MultiProcess::EndWaiting;
    task->wait_task.has_wait_task = false;
    yield();
}

void sys_read(IRQ::CSITRegisters2* frame) {
	if (frame->ebx == FD_STDIN) {
		frame->eax = Socket::read_socket(&MultiProcess::get_current_task()->stdin, frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else if ((frame->ebx & 0xff) == FD_SOCKET) {
		unsigned id = frame->ebx >> 8;
		frame->eax = Socket::read_socket(Socket::socket_from_id(id), frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else if ((frame->ebx & 0xff) == FD_FILE) {
		MultiProcess::append_wait_task(read_syscall_wait_task);
		MultiProcess::yield(frame);
	} else {
		assert(0);
	}
}