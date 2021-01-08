#include <kernel/multiprocess.hpp>
#include <kernel/kernel.hpp>
#include <assertions.h>
#include <kernel/kmalloc.hpp>
#include <kernel/hardware/gdt.hpp>
#include <kernel/hardware/pic.hpp>
#include <string.h>
#include <syscall.h>
#include <stdio.h>

MultiProcess::TSS kernel_tss;

MultiProcess::Process* current_process;

extern "C" void update_process(IRQ::CSITRegisters registers) {
	PIC::send_EOI(0);

	if (current_process->state != MultiProcess::ProcessState::Exitting) {
		memcpy(&current_process->registers, &registers, sizeof(registers));
	}

	if (current_process->state == MultiProcess::ProcessState::Running) current_process->state = MultiProcess::ProcessState::Runnable;
	current_process = current_process->next;
	// Move to the next thread
	
	while (current_process->next->state == MultiProcess::Exitting) {
		assert(current_process->next != current_process);
		current_process->next = current_process->next->next;
	}
	// Remove all of the exitting processes directly after this thread

	if (current_process->state == MultiProcess::ProcessState::Exitting) {
		assert(current_process->next != current_process);
		current_process = current_process->next;
	}
	// If this thread is also exitting it, skip it, but don't remove it (we can't)

	
	current_process->state = MultiProcess::ProcessState::Running;
	memcpy(&registers, &current_process->registers, sizeof(registers));

	MemoryManagement::load_page_dir(current_process->page_dir);
}

asm(
	".globl context_switch_interrupt_trigger\n"
	"context_switch_interrupt_trigger:\n"
		"pushl %eax\n"
		"pushl %ebx\n"
		"pushl %ecx\n"
		"pushl %edx\n"
		"pushl %esi\n"
		"pushl %edi\n"
		"pushl %ebp\n"
		
		"call update_process\n"

		"popl %ebp\n"
		"popl %edi\n"
		"popl %esi\n"
		"popl %edx\n"
		"popl %ecx\n"
		"popl %ebx\n"
		"popl %eax\n"

		"mov $0x23, %ax\n"
		"mov %ax, %ds\n"
		"mov %ax, %es\n"
		"mov %ax, %fs\n"
		"mov %ax, %gs\n"

		"iret\n"
);

void MultiProcess::end() {
	int exit_code;
	asm volatile("movl %%eax, %0" :"=b"(exit_code));
	MultiProcess::exit(current_process, exit_code);
	hang;
}

void _idle_code() {
	while (1);
}

MultiProcess::Process* MultiProcess::create(void *entry, const char *name) {
	Process* proc = (Process*) kmalloc(sizeof(Process), 0, 0);
	proc->page_dir = (MemoryManagement::PageDirectory*) kmalloc(sizeof(MemoryManagement::PageDirectory), 1, 0);
	proc->name = name;

	proc->registers.ss = 0x20 | 0x03;
	proc->registers.eflags = 0x0202;
	proc->registers.cs = 0x18 | 0x03;
	proc->registers.eip = (u32) entry;

	return proc;
}

void MultiProcess::append(Process* proc) {
	proc->next = current_process->next;
	current_process->next = proc;
	kdebugf("[MultiProcess] Process %s linked to process chain and will execute next\n", proc->name);
}

extern "C" void tss_flush();
asm(
	".globl tss_flush\n"
	"tss_flush:\n"
		"mov $0x28, %ax\n"
		"ltr %ax\n"
		"ret\n"
);

void MultiProcess::init(u32 ktss_idx, u32 kss, u32 kesp) {
	u32 base = (u32) &kernel_tss;
	GDT::set_gate(ktss_idx, base, base + sizeof(TSS), /* 0xE9 */ 0b11101001, 0);
	memset(&kernel_tss, 0, sizeof(TSS));
	kernel_tss.ss0 = kss;
	kernel_tss.esp0 = kesp;

	kernel_tss.cs = 0x0b;
    kernel_tss.ds = 0x13;
    kernel_tss.es = 0x13;
    kernel_tss.fs = 0x13;
    kernel_tss.gs = 0x13;
    kernel_tss.ss = 0x13;

	tss_flush();

	current_process = (Process*) kmalloc(sizeof(Process), 0, 0);
	current_process->name = "kernel";
	current_process->state = ProcessState::Exitting;
	current_process->next = current_process;
	current_process->page_dir = MemoryManagement::get_kernel_page_dir();

	MultiProcess::Process* idle = MultiProcess::create((void*) _idle_code, "idle");
	idle->page_dir = MemoryManagement::get_kernel_page_dir();
	MultiProcess::append(idle);
}

void MultiProcess::tss_set_stack(u32 kss, u32 kesp) {
    kernel_tss.ss0 = kss;
    kernel_tss.esp0 = kesp;
}

void MultiProcess::exit(Process* process, u32 exit_code) {
	if (process == 0) process = current_process;
	
	process->state = ProcessState::Exitting;
	kdebugf("[MultiProcess] Process %s quit with code: %d\n", process->name, exit_code);
}

void MultiProcess::yield(IRQ::CSITRegisters2* registers) {
	if (current_process->state != MultiProcess::ProcessState::Exitting) {
		memcpy(&current_process->registers, (unsigned char*)registers + 4, sizeof(IRQ::CSITRegisters));
	}

	if (current_process->state == MultiProcess::ProcessState::Running) current_process->state = MultiProcess::ProcessState::Runnable;
	current_process = current_process->next;
	// Move to the next thread
	
	while (current_process->next->state == MultiProcess::Exitting) {
		assert(current_process->next != current_process);
		current_process->next = current_process->next->next;
	}
	// Remove all of the exitting processes directly after this thread

	if (current_process->state == MultiProcess::ProcessState::Exitting) {
		assert(current_process->next != current_process);
		current_process = current_process->next;
	}
	// If this thread is also exitting it, skip it, but don't remove it (we can't)
	
	current_process->state = MultiProcess::ProcessState::Running;
	memcpy((unsigned char*) registers + 4, &current_process->registers, sizeof(IRQ::CSITRegisters));
}