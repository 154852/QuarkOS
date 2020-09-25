#include <stdint2.h>

#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

namespace GDT {
    struct __attribute__((packed)) GDTEntry {
        u16 limit_low;
        u16 base_low;
        u8 base_middle;
        u8 access;
        u8 granularity;
        u8 base_high;
    };

    struct __attribute__((packed)) GDTPointer {
        u16 limit;
        u32 base;
    };

    void set_gate(i32 idx, u32 base, u32 limit, u8 access, u8 granularity);
    void initialise();
}

#endif