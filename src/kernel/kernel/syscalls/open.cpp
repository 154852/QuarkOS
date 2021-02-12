#include "ext2/init.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <ext2/path.hpp>
#include <assertions.h>
#include <syscall.h>

void open_socket(IRQ::CSITRegisters2* frame, unsigned file, ext2::INode* inode) {	
	if (frame->ecx & FILE_FLAG_R) {
		if (file == 0) {
			frame->eax = -EFILENOTFOUND;
			return;
		}
	} else if (frame->ecx & FILE_FLAG_W) {
		assert(file != 0);
	}

	assert(((inode->type_and_permissions & 0x0000f000) == TYPE_UNIX_SOCKET));
	
	frame->eax = (inode->os_value_1 << 8) | FD_SOCKET;
}

void sys_open(IRQ::CSITRegisters2* frame) {
	if (frame->ecx & FILE_FLAG_SOCK) {
		unsigned file = ext2::inode_id_from_root_path((const char*) frame->ebx);
		open_socket(frame, file, ext2::get_tmp_inode_at(file));
	} else {
		unsigned int file = ext2::inode_id_from_root_path((const char*) frame->ebx);
		if (file == 0) {
			frame->eax = -EFILENOTFOUND;
			return;
		}

		ext2::INode* inode = ext2::get_tmp_inode_at(file);
		if (((inode->type_and_permissions & 0x0000f000) == TYPE_UNIX_SOCKET) != 0) {
			open_socket(frame, file, inode);
			return;
		}
		
		if (frame->ecx & FILE_FLAG_R) {
			assert(file < 0xffffff);
			frame->eax = (file << 8) | FD_FILE;
		} else {
			assert(0);
		}
	}
}