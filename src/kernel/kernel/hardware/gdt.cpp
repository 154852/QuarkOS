#include <kernel/hardware/gdt.hpp>

static GDT::GDTEntry gdt_entries[5];
static GDT::GDTPointer gdt_pointer;

void GDT::set_gate(i32 idx, u32 base, u32 limit, u8 access, u8 granularity) {
    gdt_entries[idx].base_low = base & 0xFFFF;
    gdt_entries[idx].base_middle = (base >> 16) & 0xFF;
    gdt_entries[idx].base_high = (base >> 24) & 0xFF;

    gdt_entries[idx].limit_low = limit & 0xFFFF;
    gdt_entries[idx].granularity = (limit >> 16) & 0x0F;

    gdt_entries[idx].granularity |= granularity & 0xF0;
    gdt_entries[idx].access = access;
}

void GDT::initialise() {
    gdt_pointer.limit = sizeof(gdt_entries) - 1;
    gdt_pointer.base = (u32) gdt_entries;

    GDT::set_gate(0, 0, 0, 0, 0);
    GDT::set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    GDT::set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    GDT::set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    GDT::set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    asm("lgdt %0" ::"m"(gdt_pointer));
    asm("mov %ax, 0x10");
    asm("mov %ds, %ax");
    asm("mov %es, %ax");
    asm("mov %fs, %ax");
    asm("mov %gs, %ax");
    asm("mov %ss, %ax");
    asm("jmp 0x08");
    asm(".init_gdt_end:");
}