#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EFILENOTFOUND 1
#define ETOOSMALL 2
#define ENOTFOUND 3

#define FD_STDIN 0x00
#define FD_STDOUT 0x01
#define FD_STDERR 0x02
#define FD_SOCKET 0x03
#define FD_FILE 0x04

typedef enum {
	SC_Read = 0x00,
	SC_FrameBufferInfo = 0x03, // TODO: These need to replaced with some kind of /dev/fb0 operation
	SC_FrameBufferSetState = 0x05,
	SC_SendIPCMessage = 0x06,
	SC_ReadIPCMessage = 0x07,
	SC_GetPid = 0x08,
	SC_FindProcPID = 0x09,
	SC_Open = 0x0a,
	SC_Write = 0x0b,
	SC_Yield = 0x0c,
	SC_Exit = 0x0d,
	SC_Exec = 0x0e,
	SC_MMap = 0x0f,
	SC_ReadDir = 0x10,
	SC_GetTime = 0x11,
	SC_GetFullTime = 0x12
} Syscall;

#define FILE_FLAG_R (1)
#define FILE_FLAG_W (1 << 1)
#define FILE_FLAG_SOCK (1 << 2)

unsigned int syscall(unsigned int type, unsigned long v1, unsigned long v2, unsigned long v3);

typedef struct {
	unsigned int year;
	unsigned int month;
	unsigned int date;

	unsigned int hour;
	unsigned int minute;
	unsigned int second;

	unsigned int weekday;
} FullTime;

void get_time(unsigned long long* time);
void get_full_time(FullTime* time);

// TODO: Ideally this would be a size in bytes
void mmap(void* addr, unsigned long pages);

void write(unsigned fd, const void *string, unsigned long length);
unsigned read(unsigned fd, void *string, unsigned long length);
unsigned open(const char *string, unsigned flags);

typedef enum {
	FT_Socket,
	FT_File,
	FT_Directory
} FileType;

typedef struct {
	char name[64];
	FileType type;
} DirEntry;

int read_dir(char* path, DirEntry* entries, unsigned long count);

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

typedef struct {
	unsigned* framebuffer;
	unsigned int size;
	unsigned short width;
	unsigned short height;
	char enabled;
} FrameBufferInfo;

void framebuffer_info(FrameBufferInfo* info);
void framebuffer_set_state(FrameBufferInfo* info);

void send_ipc_message(unsigned target_pid, void* raw, unsigned length);
unsigned read_ipc_message(void* raw, unsigned length, unsigned* sender);

unsigned get_pid();
unsigned find_proc_pid(char* name);

unsigned int exec(const char *path, const char** argv, int argc);
void yield();
void __attribute__((noreturn)) exit(unsigned char code);

#ifdef __cplusplus
}
#endif

#endif