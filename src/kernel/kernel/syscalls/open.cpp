#include "ext2/init.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <ext2/path.hpp>
#include <assertions.h>
#include <syscall.h>

void sys_open(IRQ::CSITRegisters2* frame) {
	if (frame->ecx & FILE_FLAG_SOCK) {
		unsigned int file = ext2::inode_id_from_root_path((const char*) frame->ebx);
		
		if (frame->ecx & FILE_FLAG_R) {
			assert(file != 0);
		} else if (frame->ecx & FILE_FLAG_W) {
			if (file == 0) {
				assert(false);
			}
		}
		
		ext2::INode* inode = ext2::get_tmp_inode_at(file);
		frame->eax = (inode->os_value_1 << 8) | FD_SOCKET;
	} else {
		unsigned int file = ext2::inode_id_from_root_path((const char*) frame->ebx);
		if (file == 0) {
			frame->eax = -EFILENOTFOUND;
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