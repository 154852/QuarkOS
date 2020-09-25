#include <kernel/kmalloc.h>

extern u32 kernel_end;
u32 placement_address = 0;

u32 kmalloc(u32 size, int align, u32* physical) {
    if (placement_address == 0) placement_address = (u32) &kernel_end;

    if (align == 1 && (placement_address & 0x00000FFF)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    if (physical) *physical = placement_address;

    u32 tmp = placement_address;
    placement_address += size;
    return tmp;
}

u32 kmalloc_a(u32 size) {
    return kmalloc(size, 1, 0);
}

u32 kmalloc_p(u32 size, u32* physical) {
    return kmalloc(size, 0, physical);
}

u32 kmalloc_ap(u32 size, u32* physical) {
    return kmalloc(size, 1, physical);
}

u32 kmalloc_placement_address() {
    return placement_address;
}