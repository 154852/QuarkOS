#include <stdint2.h>

#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} GDTEntry;

typedef struct __attribute__((packed)) {
    u16 limit;
    u32 base;
} GDTPointer;

void init_gdt();

#ifdef __cplusplus
}
#endif

#endif