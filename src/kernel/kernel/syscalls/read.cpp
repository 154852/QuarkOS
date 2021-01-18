#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/keyboard.hpp>

#include <assertions.h>
#include <syscall.h>
#include <string.h>

// void read_syscall_wait_task() {
//     MultiProcess::Process* task = MultiProcess::get_current_task();
//     assert(task->registers.edx == 1);

//     while (Keyboard::get_buffer_size() < task->registers.edx) {
//         yield();
//     }

//     memcpy((void*) task->registers.ecx, Keyboard::get_buffer(), task->registers.edx);
//     memmove(Keyboard::get_buffer(), Keyboard::get_buffer() + 1, Keyboard::get_buffer_size());
//     Keyboard::pop_from_buffer(task->registers.edx);
//     task->state = MultiProcess::EndWaiting;
//     task->wait_task.has_wait_task = false;
//     yield();
// }

void sys_read(IRQ::CSITRegisters2* frame) {
	if (frame->ebx == FD_STDIN) {
		frame->eax = Socket::read_socket(&MultiProcess::get_current_task()->stdin, frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else if ((frame->ebx & 0xff) == FD_SOCKET) {
		unsigned id = frame->ebx >> 8;
		frame->eax = Socket::read_socket(Socket::socket_from_id(id), frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else {
		assert(0);
	}
}