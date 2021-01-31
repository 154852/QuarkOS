#include "kernel/ustar.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <assertions.h>
#include <syscall.h>

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
		frame->eax = (socket->id << 8) | FD_SOCKET;
	} else {
		 unsigned int file = USTAR::lookup_raw_pointer((const char*) frame->ebx);
		if (file == 0) {
			frame->eax = -EFILENOTFOUND;
			return;
		}

		if (frame->ecx & FILE_FLAG_R) {
			assert(file < 0xffffff); // TODO: this will not work pretty soon
			frame->eax = (file << 8) | FD_FILE;
		} else {
			assert(0);
		}
	}
}