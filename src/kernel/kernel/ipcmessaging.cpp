#include <kernel/kernel.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/ipcmessaging.hpp>
#include <kernel/multiprocess.hpp>
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

			MultiProcess::Process* proc = MultiProcess::find_process_by_pid(target_pid);

			proc->definitely_has_noipcmsg = false;

			return;
		}
	}

	assert(false);
}

IPCMessaging::Message IPCMessaging::read_message(u32 to_pid) {
	MultiProcess::Process* proc = MultiProcess::find_process_by_pid(to_pid);

	if (proc->definitely_has_noipcmsg) {
		IPCMessaging::Message failed;
		failed.present = false;
		return failed;
	}

	for (int i = 0; i < MESSAGES_CAPACITY; i++) {
		if (messages[i].present && messages[i].target_pid == to_pid){
			IPCMessaging::Message copy;
			memcpy(&copy, &messages[i], sizeof(Message));
			proc->definitely_has_noipcmsg = false;
			messages[i].present = false;
			return copy;
		}
	}

	proc->definitely_has_noipcmsg = true;

	IPCMessaging::Message failed;
	failed.present = false;
	return failed;
}