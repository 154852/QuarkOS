#include <stdint2.h>

#ifndef _KERNEL_KMALLOC_H
#define _KERNEL_KMALLOC_H

u32 kmalloc(u32 size, int align, u32* physical);
u32 kmalloc_a(u32 size);
u32 kmalloc_p(u32 size, u32* physical);
u32 kmalloc_ap(u32 size, u32* physical);
u32 kmalloc_placement_address();

#endif