#include <stdlib.h>
#include <syscall.h>
#include <assertions.h>

// TODO: I am well aware of just how terrible this whole malloc system is, but it's simple and works for now

#ifdef __is_kernel
#include <stdio.h>
#endif

#define MALLOC_START (void*) 0xD0000000
#define PAGE_SIZE 0x1000
size_t malloc_used = 0;

typedef struct {
	void* addr;
	size_t pages;
} Hole;

#define HOLES_CAPACITY 2048
static Hole holes[HOLES_CAPACITY];

void* malloc(size_t size) {
#ifdef __is_kernel
	debugf("Cannot malloc/free inside the kernel, use kmalloc instead.\n");
	while (1);
#endif

	size_t pages = (size % PAGE_SIZE == 0)? (size / PAGE_SIZE):((size / PAGE_SIZE) + 1);

	for (int i = 0; i < HOLES_CAPACITY; i++) {
		if (holes[i].addr != 0 && holes[i].pages >= pages) {
			void* addr = holes[i].addr;
			holes[i].addr = 0;
			return addr;
		}
	}

	void* addr = MALLOC_START + malloc_used;
	malloc_used += pages * PAGE_SIZE;
	mmap(addr, pages);
	return addr;
}

void free_sized(void* addr, size_t size) {
#ifdef __is_kernel
	debugf("Cannot malloc/free inside the kernel, use kmalloc instead (which cannot be freed).\n");
	while (1);
#endif

	size_t pages = (size % PAGE_SIZE == 0)? (size / PAGE_SIZE):((size / PAGE_SIZE) + 1);

	for (int i = 0; i < HOLES_CAPACITY; i++) {
		if (holes[i].addr == 0) {
			holes[i].addr = addr;
			holes[i].pages = pages;
			return;
		}
	}

	assert(0);
}