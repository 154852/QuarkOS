#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFILENOTFOUND 1
#define ETOOSMALL 2
#define ENOTFOUND 3

typedef enum {
	SC_Read = 0x00,
	SC_Write = 0x04,
	SC_Yield = 0x17,
	SC_Exit = 0x37,
	SC_Exec = 0x3b,
	SC_ProcInfo = 0x01,
	SC_LSProc = 0x02,
	SC_FrameBufferInfo = 0x03,
	SC_FrameBufferSetState = 0x05,
	SC_SendIPCMessage = 0x06,
	SC_ReadIPCMessage = 0x07,
	SC_GetPid = 0x08,
	SC_FindProcPID = 0x09,
} Syscall;

unsigned int syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3);
void write(const char *string, unsigned long length);
void read(char *string, unsigned long length);
unsigned int list_process_pids(int *pids, unsigned long length);

typedef enum {
	PSSC_NotPresent,
	PSSC_Exitting,
	PSSC_Running,
	PSSC_Idle
} ProcessStateSC;

typedef struct {
	unsigned int pid;

	ProcessStateSC state;
	char name[64];
} ProcessInfo;

void proc_info(ProcessInfo *info);

typedef struct {
	unsigned* framebuffer;
	unsigned int size;
	unsigned short width;
	unsigned short height;
	char enabled;
} FrameBufferInfo;

void framebuffer_info(FrameBufferInfo* info);
void framebuffer_set_state(FrameBufferInfo* info);

void send_ipc_message(unsigned target_pid, char* raw, unsigned length);
unsigned read_ipc_message(char* raw, unsigned length, unsigned* sender);

unsigned get_pid();
unsigned find_proc_pid(char* name);

unsigned int exec(const char *path);
void yield();
void __attribute__((noreturn)) exit(unsigned char code);

#ifdef __cplusplus
}
#endif

#endif