#include <stdlib.h>

// TODO: This is a work in progress, it isn't used yet, and wouldn't work anyway

#ifdef __is_libk
#include <stdio.h>
#endif

#define MALLOC_START (void*) 0xD0000000
#define PAGE_SIZE 0x1000
size_t malloc_used = 0;

void mmap(void* addr, size_t size) {

}

void* malloc(size_t size) {
#ifdef __is_libk
	debugf("Cannot malloc/free inside the kernel, use kmalloc instead.\n");
	while (1);
#endif

	void* addr = MALLOC_START + malloc_used;

	if (size < PAGE_SIZE) {
		malloc_used += PAGE_SIZE;
		mmap(addr, PAGE_SIZE);
		return addr;
	}

	size = (size % PAGE_SIZE == 0)? (size / PAGE_SIZE):((size / PAGE_SIZE) + 1);
	malloc_used += size;
	mmap(addr, size);
	return addr;
}

void free(void* addr) {
#ifdef __is_libk
	debugf("Cannot malloc/free inside the kernel, use kmalloc instead (which cannot be freed).\n");
	while (1);
#endif
}