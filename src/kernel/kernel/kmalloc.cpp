#include <kernel/kmalloc.hpp>
#include <assertions.h>
#include <kernel/paging.hpp>

extern u32 kernel_end;
u32 placement_address = 0;

void* _kmalloc(u32 size, int align) {
    if (placement_address == 0) placement_address = (u32) &kernel_end;

    if (align == 1 && (placement_address & 0x00000FFF)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }

    u32 tmp = placement_address;
    placement_address += size;
    assert(placement_address < KERNEL_SIZE);
    return (void*) tmp;
}

void* kmalloc(size_t size) {
    return _kmalloc(size, 0);
}

void* kmalloc_aligned(size_t size) {
    return _kmalloc(size, 1);
}

u32 kmalloc_placement_address() {
    return placement_address;
}