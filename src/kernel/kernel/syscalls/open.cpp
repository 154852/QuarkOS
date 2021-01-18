#include "kernel/ustar.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <assertions.h>
#include <syscall.h>

void open_syscall_wait_task() {
    MultiProcess::Process* task = MultiProcess::get_current_task();

    unsigned int file = USTAR::lookup_raw_pointer((const char*) task->registers.ebx);
    if (file == 0) {
        task->registers.eax = -EFILENOTFOUND;
        task->state = MultiProcess::EndWaiting;
        task->wait_task.has_wait_task = false;
        yield();
        return;
    }

	if (task->registers.ecx & FILE_FLAG_R) {
		assert(file < 0xffffff); // TODO: this will not work pretty soon
		task->registers.eax = (file << 8) | FD_FILE;
	} else {
		assert(0);
	}

    task->state = MultiProcess::EndWaiting;
    task->wait_task.has_wait_task = false;
    yield();
}


void sys_open(IRQ::CSITRegisters2* frame) {
	if (frame->ecx & FILE_FLAG_SOCK) {
		Socket::Socket* socket = Socket::open_socket(reinterpret_cast<char*>(frame->ebx));
		if (frame->ecx & FILE_FLAG_R) {
			assert(socket != 0);
		} else if (frame->ecx & FILE_FLAG_W) {
			if (socket == 0) {
				socket = Socket::new_socket(reinterpret_cast<char*>(frame->ebx));
			}
		} else {
			assert(false);
		}
		frame->eax = (socket->id << 8) | FD_SOCKET; // 0x00 for stdin, 0x01 for stdout, 0x02 for stderr, 0x03 for socket, 0x04 for file
	} else {
		MultiProcess::append_wait_task(open_syscall_wait_task);
		MultiProcess::yield(frame);
	}
}