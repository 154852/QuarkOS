#include <stdint2.h>

#ifndef _KERNEL_KMALLOC_H
#define _KERNEL_KMALLOC_H

void* kmalloc(u32 size, int align, u32* physical);
void* kmalloc_a(u32 size);
void* kmalloc_p(u32 size, u32* physical);
void* kmalloc_ap(u32 size, u32* physical);
void* kmalloc_placement_address();

#endif