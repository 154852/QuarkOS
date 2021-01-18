#include <stdint2.h>
#include <stddef.h>

#ifndef _KERNEL_KMALLOC_H
#define _KERNEL_KMALLOC_H

void* kmalloc(size_t size);
void* kmalloc_aligned(size_t size);

u32 kmalloc_placement_address();

#endif