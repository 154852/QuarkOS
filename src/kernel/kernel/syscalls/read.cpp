#include <ext2/path.hpp>
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/keyboard.hpp>

#include <assertions.h>
#include <syscall.h>
#include <string.h>

void sys_read(IRQ::CSITRegisters2* frame) {
	if (frame->ebx == FD_STDIN) {
		frame->eax = Socket::read_socket(&MultiProcess::get_current_task()->stdin, frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else if ((frame->ebx & 0xff) == FD_SOCKET) {
		unsigned id = frame->ebx >> 8;
		frame->eax = Socket::read_socket(Socket::socket_from_id(id), frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else if ((frame->ebx & 0xff) == FD_FILE) {
		unsigned id = frame->ebx >> 8;
		ext2::INode* node = ext2::get_tmp_inode_at(id);
		unsigned size = frame->edx > node->size_low? node->size_low:frame->edx;
		ext2::read_file_content(node, 0, (void*) frame->ecx, size);
		frame->eax = size;
	} else {
		assert(0);
	}
}