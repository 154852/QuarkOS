#include <kernel/tty.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/socket.hpp>
#include <kernel/hardware/BXVGA.hpp>
#include <assertions.h>
#include <syscall.h>

void sys_write(IRQ::CSITRegisters2* frame) {
	if (frame->ebx == FD_STDOUT) {
		Socket::write_socket(&MultiProcess::get_current_task()->stdout, frame->edx, reinterpret_cast<void*>(frame->ecx));

		if (BXVGA::is_enabled()) debugf("%.*s\n", frame->edx, reinterpret_cast<const char*>(frame->ecx));
		else Terminal::write(reinterpret_cast<const char*>(frame->ecx), frame->edx);
	} else if (frame->ebx == FD_STDERR) {
		Socket::write_socket(&MultiProcess::get_current_task()->stderr, frame->edx, reinterpret_cast<void*>(frame->ecx));

		if (BXVGA::is_enabled()) debugf("[ERROR] %.*s\n", frame->edx, reinterpret_cast<const char*>(frame->ecx));
		else Terminal::write(reinterpret_cast<const char*>(frame->ecx), frame->edx);
	} else if ((frame->ebx & 0xff) == FD_SOCKET) {
		unsigned id = frame->ebx >> 8;
		Socket::write_socket(Socket::socket_from_id(id), frame->edx, reinterpret_cast<void*>(frame->ecx));
	} else {
		assert(0);
	}
}