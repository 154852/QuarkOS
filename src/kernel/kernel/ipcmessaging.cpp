#include <kernel/kernel.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/ipcmessaging.hpp>
#include <stdio.h>
#include <assertions.h>
#include <string.h>

#define MESSAGES_CAPACITY 512
static IPCMessaging::Message messages[MESSAGES_CAPACITY];

void IPCMessaging::send_message(u32 from_pid, u32 target_pid, void* raw, size_t size) {
	for (int i = 0; i < MESSAGES_CAPACITY; i++) {
		if (!messages[i].present) {
			messages[i].present = true;
			messages[i].from_pid = from_pid;
			messages[i].raw = raw;
			messages[i].size = size;
			messages[i].target_pid = target_pid;

			return;
		}
	}

	assert(false);
}

IPCMessaging::Message IPCMessaging::read_message(u32 to_pid) {
	for (int i = 0; i < MESSAGES_CAPACITY; i++) {
		if (messages[i].present && messages[i].target_pid == to_pid){
			IPCMessaging::Message copy;
			memcpy(&copy, &messages[i], sizeof(Message));
			messages[i].present = false;
			return copy;
		}
	}

	IPCMessaging::Message failed;
	failed.present = false;
	return failed;
}