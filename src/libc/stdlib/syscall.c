#include <syscall.h>
#include <stdio.h>

unsigned int syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3) {
#ifdef DEBUG_INT
	debugf("[LibC] SYSCALL\n");
	debugf("EAX=%u\nEBX=%u\nECX=%u\nEDX=%u\n\n", type, v1, v2, v3);
#endif

	unsigned int ret;
	asm volatile(
		"int $0x80\n"
		"movl %%eax, %0\n"
		:"=m"(ret)
		: "a"(type), "b"(v1), "c"(v2), "d"(v3)
	);

	return ret;
}

void mmap(void* addr, unsigned long pages) {
	syscall(SC_MMap, (unsigned long) addr, pages, 0);
}

void proc_info(ProcessInfo *info) {
	syscall(SC_ProcInfo, (unsigned long) info, 0, 0);
}

void framebuffer_info(FrameBufferInfo* info) {
	syscall(SC_FrameBufferInfo, (unsigned long) info, 0, 0);
}

void framebuffer_set_state(FrameBufferInfo* info) {
	syscall(SC_FrameBufferSetState, (unsigned long) info, 0, 0);
}

void write(unsigned fd, const void* string, unsigned long length) {
	syscall(SC_Write, fd, (unsigned long) string, length);
}

unsigned read(unsigned fd, void* string, unsigned long length) {
	return syscall(SC_Read, fd, (unsigned long) string, length);
}

unsigned open(char *string, unsigned flags) {
	return syscall(SC_Open, (unsigned long) string, flags, 0);
}

unsigned int exec(const char* path) {
	return syscall(SC_Exec, (unsigned long) path, 0, 0);
}

void yield() {
	 syscall(SC_Yield, 0, 0, 0);
}

void exit(unsigned char code) {
	 syscall(SC_Exit, code, 0, 0);
	 while (1);
}

unsigned int list_process_pids(int* pids, unsigned long length) {
	return syscall(SC_LSProc, (unsigned long) pids, length, 0);
}

void send_ipc_message(unsigned target_pid, void* raw, unsigned length) {
	syscall(SC_SendIPCMessage, target_pid, (unsigned) raw, length);
}

unsigned read_ipc_message(void* raw, unsigned length, unsigned* sender) {
	return syscall(SC_ReadIPCMessage, (unsigned) raw, length, (unsigned) sender);
}

unsigned get_pid() {
	return syscall(SC_GetPid, 0, 0, 0);
}

unsigned find_proc_pid(char* name) {
	return syscall(SC_FindProcPID, (unsigned) name, 0, 0);
}