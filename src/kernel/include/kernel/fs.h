// #include <kernel/interrupts.h>
// #include <stddef.h>
// #include <string.h>

// #ifndef _KERNEL_FS_H
// #define _KERNEL_FS_H

// typedef struct {
// 	size_t offset;
// 	size_t size;
// } FileDescriptor;

// static FileDescriptor open_files[10];
// static size_t open_files_count = 0;

// int open(const char* pathname, int flags) {
// 	if (strcmp(pathname, "example.elf.bin")) {
// 		open_files[open_files_count].offset = 0;
// 		open_files[open_files_count].size = 
// 		return open_files_count++;
// 	}

// 	return -1;
// }

// void read(int fd, void *buf, size_t count) {

// }

// #define SEEK_SET 0
// #define SEEK_CUR 1
// #define SEEK_END 2

// void lseek(int fd, int offset, int whence) {
// 	open_files[fd].offset
// }

// // __attribute__((interrupt)) void read_isr(struct interrupt_frame* frame) {

// // }

// #endif