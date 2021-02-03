#include <syscall.h>
#include <stdio.h>

unsigned int syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3) {
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

unsigned open(const char *string, unsigned flags) {
	return syscall(SC_Open, (unsigned long) string, flags, 0);
}

unsigned int exec(const char* path) {
	return syscall(SC_Exec, (unsigned long) path, 0, 0);
}

int read_dir(char* path, DirEntry* entries, unsigned long count) {
	return (int) syscall(SC_ReadDir, (unsigned long) path, (unsigned long) entries, count);
}

void yield() {
	 syscall(SC_Yield, 0, 0, 0);
}

void exit(unsigned char code) {
	 syscall(SC_Exit, code, 0, 0);
	 while (1);
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