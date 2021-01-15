#include <stddef.h>
#include <stdint2.h>

#ifndef _KERNEL_IPCMESSAGING_H
#define _KERNEL_IPCMESSAGING_H

#define SOCKET_CHUNK_COUNT 1024
#define SOCKET_NAME_SIZE 64

namespace IPCMessaging {
	struct Message {
		bool present = false;

		u32 from_pid = 0;
		u32 target_pid = 0;
		char* raw = NULL;
		size_t size = 0;
	};

	void send_message(u32 from_pid, u32 target_pid, char* raw, size_t size);
	Message read_message(u32 to_pid);
};

#endif