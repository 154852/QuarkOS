#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <assertions.h>
#include <syscall.h>

void sys_open(IRQ::CSITRegisters2* frame) {
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
}