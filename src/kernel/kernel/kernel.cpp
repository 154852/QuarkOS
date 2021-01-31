#include <assertions.h>
#include <kernel/vga.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/hardware/cmos.hpp>
#include <kernel/hardware/pit.hpp>
#include <kernel/hardware/interrupts.hpp>
#include <kernel/hardware/keyboard.hpp>
#include <kernel/ipcmessaging.hpp>
#include <kernel/hardware/BXVGA.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/hardware/pci.hpp>
#include <kernel/paging.hpp>
#include <kernel/hardware/gdt.hpp>
#include <kernel/hardware/mouse.hpp>
#include <kernel/hardware/disk.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/ustar.hpp>
#include <stdio.h>
#include <kernel/tty.hpp>
#include <stdint2.h>
#include <string.h>
#include <kernel/elf.hpp>
#include <syscall.h>
#include <kernel/kernel.hpp>
#include <kernel/socket.hpp>
#include <kernel/syscall.hpp>

extern "C" u16 exception_code;
asm(
    ".globl exception_code\n"
    "exception_code:\n"
    ".short 0\n"
);

#define full_state_dump_interrupt(name)\
extern "C" void name ## _interrupt_trigger();\
asm( \
	".globl " #name "_interrupt_trigger\n" \
	#name  "_interrupt_trigger:\n" \
        "cli\n" \
        \
		"pushl %eax\n" \
		"pushl %ebx\n" \
		"pushl %ecx\n" \
		"pushl %edx\n" \
		"pushl %esi\n" \
		"pushl %edi\n" \
		"pushl %ebp\n" \
        \
        "mov %ds, %ax\n"\
        "pushl %eax\n" \
        \
        "mov $0x10, %ax\n" \
        "mov %ax, %ds\n" \
        "mov %ax, %es\n" \
        "mov %ax, %fs\n" \
        "mov %ax, %gs\n" \
		\
        "pushl %esp\n" \
		"call " #name "_handle\n" \
        "popl %esp\n" \
        \
        "popl %ebx\n" \
        "mov %bx, %ds\n" \
        "mov %bx, %es\n" \
        "mov %bx, %fs\n" \
        "mov %bx, %gs\n" \
        \
		"popl %ebp\n" \
		"popl %edi\n" \
		"popl %esi\n" \
		"popl %edx\n" \
		"popl %ecx\n" \
		"popl %ebx\n" \
		"popl %eax\n" \
        \
        "sti\n" \
        \
		"iret\n" \
);

#define full_state_dump_interrupt_with_code(name)\
extern "C" void name ## _interrupt_trigger();\
asm( \
	".globl " #name "_interrupt_trigger\n" \
	#name  "_interrupt_trigger:\n" \
        "pop exception_code\n"\
		"pushl %eax\n" \
		"pushl %ebx\n" \
		"pushl %ecx\n" \
		"pushl %edx\n" \
		"pushl %esi\n" \
		"pushl %edi\n" \
		"pushl %ebp\n" \
		\
		"call " #name "_handle\n" \
        \
		"popl %ebp\n" \
		"popl %edi\n" \
		"popl %esi\n" \
		"popl %edx\n" \
		"popl %ecx\n" \
		"popl %ebx\n" \
		"popl %eax\n" \
        \
		"mov $0x23, %ax\n" \
		"mov %ax, %ds\n" \
		"mov %ax, %es\n" \
		"mov %ax, %fs\n" \
		"mov %ax, %gs\n" \
        \
		"iret\n" \
);

typedef void(*SyscallHandler)(IRQ::CSITRegisters2* frame);
SyscallHandler syscall_table[0xff];

// NOTE: We return int (even though we have no desire to return anything) to prevent gcc tail end optimising function calls, which somehow breaks things
extern "C" int syscall_handle(IRQ::CSITRegisters2* frame) {
    if (frame->eax > 0xff || syscall_table[frame->eax] == 0) {
        kdebugf("[Core::Syscall] Unknown syscall: %.2x\n", frame->eax);
        hang;
    }

    assert(MultiProcess::get_current_task()->ring == 3);

    PIC::irq_set_mask(0);
    asm("sti");

    MultiProcess::get_current_task()->ring = 0;
    syscall_table[frame->eax](frame);
    MultiProcess::get_current_task()->ring = 3;

    asm("cli");
    PIC::irq_clear_mask(0);
    return 0;
}
full_state_dump_interrupt(syscall);

extern "C" void page_fault_handle(IRQ::CSITRegisters regs) {
    IRQ::disable_irq();

    u32 fault_addr; asm ("movl %%cr2, %%eax":"=a"(fault_addr));

    debugf("Page Fault in attempt to access 0x%x : %s\n", fault_addr, MultiProcess::get_current_task()->name);

    debugf("exception code: %i\n", exception_code);
    debugf("pc=%x:%x\n", regs.cs, regs.eip);
    debugf("eax=%x ebx=%x ecx=%x edx=%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
    debugf("ebp=%x esp=%x esi=%x edi=%x\n", regs.ebp, regs.esp, regs.esi, regs.edi);
    hang;
}
full_state_dump_interrupt_with_code(page_fault);

extern "C" void bounds_check_handle(IRQ::CSITRegisters regs) {
    IRQ::disable_irq();

    debugf("bounds_check failure\n");

    debugf("exception code: %i\n", exception_code);
    debugf("pc=%x:%x\n", regs.cs, regs.eip);
    debugf("eax=%x ebx=%x ecx=%x edx=%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
    debugf("ebp=%x esp=%x esi=%x edi=%x\n", regs.ebp, regs.esp, regs.esi, regs.edi);
    hang;
}
full_state_dump_interrupt_with_code(bounds_check);

extern "C" void general_protection_fault_handle(IRQ::CSITRegisters regs) {
    IRQ::disable_irq();

    debugf("General protection fault in : %s\n", MultiProcess::get_current_task()->name);

    debugf("exception code: %i\n", exception_code);
    debugf("pc=%x:%x\n", regs.cs, regs.eip);
    debugf("eax=%x ebx=%x ecx=%x edx=%x\n", regs.eax, regs.ebx, regs.ecx, regs.edx);
    debugf("ebp=%x esp=%x esi=%x edi=%x\n", regs.ebp, regs.esp, regs.esi, regs.edi);
    hang;
}
full_state_dump_interrupt_with_code(general_protection_fault);

extern "C" void double_fault_handle() {
    IRQ::disable_irq();
    kdebugf("Double Fault\n");
    hang;
}
full_state_dump_interrupt_with_code(double_fault);

#define QUICK_INTERRUPT(name) __attribute__((interrupt)) void name(struct interrupt_frame*, unsigned long) { kdebugf(#name "\n"); hang; }

QUICK_INTERRUPT(division_error);
QUICK_INTERRUPT(debug_exception);
QUICK_INTERRUPT(unknown_error);
QUICK_INTERRUPT(breakpoint);
QUICK_INTERRUPT(overflow);
QUICK_INTERRUPT(invalid_opcode);
QUICK_INTERRUPT(coprocess_not_available);
QUICK_INTERRUPT(coprocessor_segment_overrun);
QUICK_INTERRUPT(invalid_tss);
QUICK_INTERRUPT(segment_not_present);
QUICK_INTERRUPT(stack_exception);
QUICK_INTERRUPT(unknown_error2);
QUICK_INTERRUPT(coprocessor_error);

void* specific_interrupt_handlers[256];

u32 stack_top;

extern "C" void kernel_main(void) {
    asm volatile("mov %%esp, %0" : "=m"(stack_top));

    Terminal::initialize();

    kdebugf("[Core] Starting QuarkOS\n");

    PIC::remap(0x20, 0x28);
    PIC::irq_clear_mask(2); // Required
    kdebugf("[Core] Initiliased PIC\n");

    GDT::initialise();
    kdebugf("[Core] Initiliased GDT\n");

    specific_interrupt_handlers[0x00] = (void*) division_error;
    specific_interrupt_handlers[0x01] = (void*) debug_exception;
    specific_interrupt_handlers[0x02] = (void*) unknown_error;
    specific_interrupt_handlers[0x03] = (void*) breakpoint;
    specific_interrupt_handlers[0x04] = (void*) overflow;
    specific_interrupt_handlers[0x05] = (void*) bounds_check_interrupt_trigger;
    specific_interrupt_handlers[0x06] = (void*) invalid_opcode;
    specific_interrupt_handlers[0x07] = (void*) coprocess_not_available;
    specific_interrupt_handlers[0x08] = (void*) double_fault_interrupt_trigger;
    specific_interrupt_handlers[0x09] = (void*) coprocessor_segment_overrun;
    specific_interrupt_handlers[0x0A] = (void*) invalid_tss;
    specific_interrupt_handlers[0x0B] = (void*) segment_not_present;
    specific_interrupt_handlers[0x0C] = (void*) stack_exception;
    specific_interrupt_handlers[0x0D] = (void*) general_protection_fault_interrupt_trigger;
    specific_interrupt_handlers[0x0E] = (void*) page_fault_interrupt_trigger;
    specific_interrupt_handlers[0x0F] = (void*) unknown_error2;
    specific_interrupt_handlers[0x10] = (void*) coprocessor_error;

    specific_interrupt_handlers[0x20] = (void*) MultiProcess::context_switch_interrupt_trigger;
    specific_interrupt_handlers[0x21] = (void*) Keyboard::keyboard_interrupt;
    specific_interrupt_handlers[0x20 + 12] = (void*) Mouse::mouse_interrupt;
    specific_interrupt_handlers[0x80] = (void*) syscall_interrupt_trigger;
    specific_interrupt_handlers[0x2E] = (void*) Disk::disk_interrupt;
    IRQ::interrupts_initialise((IRQ::GenericInterruptHandler*) specific_interrupt_handlers);
    kdebugf("[Core] Initiliased interrupts\n");

    syscall_table[SC_Read] = sys_read;
    syscall_table[SC_FrameBufferInfo] = sys_framebuffer_info;
    syscall_table[SC_FrameBufferSetState] = sys_framebuffer_set_state;
    syscall_table[SC_SendIPCMessage] = sys_send_ipc_message;
    syscall_table[SC_ReadIPCMessage] = sys_read_ipc_message;
    syscall_table[SC_GetPid] = sys_get_pid;
    syscall_table[SC_FindProcPID] = sys_find_proc_by_id;
    syscall_table[SC_Open] = sys_open;
    syscall_table[SC_Write] = sys_write;
    syscall_table[SC_Yield] = sys_yield;
    syscall_table[SC_Exit] = sys_exit;
    syscall_table[SC_Exec] = sys_exec;
    syscall_table[SC_MMap] = sys_mmap;
    syscall_table[SC_ReadDir] = sys_readdir;

    PCI::load_hardware();
    kdebugf("[Core] Initialised PCI\n");
    BXVGA::initialise();
    kdebugf("[Core] Initialised BXVGA\n");

    IRQ::enable_irq();
    Disk::initialise();
    kdebugf("[Core] Initiliased disk\n");
    IRQ::disable_irq();

    Mouse::init();
    Keyboard::init();
    
    MemoryManagement::init_paging();
    kdebugf("[Core] Initiliased paging\n");

    kdebugf("[Core] Disable IRQ\n");
    MultiProcess::init(5, 0x10, 0);
    kdebugf("[Core] Multiprocess init\n");

    IRQ::enable_irq();

    PIC::irq_clear_mask(1); // Keyboard
    PIC::irq_clear_mask(12);
    PIT::attempt_to_set_frequency(400);

    kdebugf("[Core] Stack top = %.8x\n", stack_top);
    MultiProcess::tss_set_stack(0x10, stack_top);

    USTAR::FileParsed* file = USTAR::lookup_parsed("/usr/bin/windowserver");
    assert(file);
    MultiProcess::Process* proc = ELF::load_static_source(file->content, file->length, MultiProcess::create(0, "/usr/bin/windowserver"));
    // TODO: There's probably a neater way to do this - maybe with some kind /dev/fb0 or something?
    MemoryManagement::identity_map_region(proc->page_dir, (u32) BXVGA::framebuffer(), BXVGA::framebuffer_size(), false, true, true);
    MultiProcess::append(proc);

    kdebugf("[Core] Starting clock...\n");
    PIC::irq_clear_mask(0); // PIT

    for(;;) { asm("hlt"); }
}