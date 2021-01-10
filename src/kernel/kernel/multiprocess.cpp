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

MultiProcess::Process* MultiProcess::get_current_task() {
	return current_process;
}

MultiProcess::Process* find_next_task() {
	if (current_process->state == MultiProcess::ProcessState::Running) current_process->state = MultiProcess::ProcessState::Runnable;
	current_process = current_process->next;
	// Move to the next thread
	
	while (current_process->next->state == MultiProcess::ProcessState::Exitting) {
		assert(current_process->next != current_process);
		current_process->next = current_process->next->next;
	}
	// Remove all of the exitting processes directly after this thread

	if (current_process->state == MultiProcess::ProcessState::Exitting) {
		assert(current_process->next != current_process);
		current_process = current_process->next;
	}
	// If this thread is also exitting it, skip it, but don't remove it (we can't)

	// TODO: Don't run the idle process unless we absolutely have to
	// if (current_process->state == MultiProcess::Idle) current_process = current_process->next;

	return current_process;
}

extern "C" int update_process(IRQ::CSITRegisters2* registers) {
	PIC::send_EOI(0);
	MultiProcess::yield(registers);
	return 0;
}

void MultiProcess::yield(IRQ::CSITRegisters2* registers) {
	IRQ::CSITRegisters* original_registers = (IRQ::CSITRegisters*) ((unsigned char*) registers + 4);

	if (current_process->state != MultiProcess::ProcessState::Exitting && current_process->state != MultiProcess::ProcessState::EndWaiting) {
		if (current_process->state == MultiProcess::ProcessState::BeginWaiting) {
			memcpy(&current_process->registers, original_registers, sizeof(IRQ::CSITRegisters));
			current_process->state = MultiProcess::ProcessState::Waiting;
		} else if (current_process->state == MultiProcess::ProcessState::Waiting) {
			memcpy(&current_process->wait_task.registers, original_registers, sizeof(IRQ::CSITRegisters));
		} else {
			memcpy(&current_process->registers, original_registers, sizeof(IRQ::CSITRegisters));
		}
	}

	/* current_process = */ find_next_task();
	
	if (current_process->state == MultiProcess::ProcessState::Waiting) {
		memcpy(original_registers, &current_process->wait_task.registers, sizeof(IRQ::CSITRegisters));
	} else {
		if (current_process->state == MultiProcess::ProcessState::Runnable) current_process->state = MultiProcess::ProcessState::Running;
		memcpy(original_registers, &current_process->registers, sizeof(IRQ::CSITRegisters));
	}

	MemoryManagement::load_page_dir(current_process->page_dir);

	if (current_process->ring0_request.has_ring0_request) {
		current_process->ring0_request.callback(current_process->ring0_request.data);
	}
}

asm (
	".globl context_switch_interrupt_trigger\n" \
	"context_switch_interrupt_trigger:\n" \
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
		"call update_process\n" \
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
		"iret\n" \
);

void MultiProcess::append_wait_task(MultiProcess::WaitTaskCallback callback) {
	current_process->wait_task.has_wait_task = true;
	current_process->wait_task.registers.eip = (u32) callback;
	current_process->wait_task.registers.ebp = current_process->wait_task.ebp;
	current_process->wait_task.registers.esp = current_process->wait_task.ebp + (4 * KB);
	current_process->state = MultiProcess::ProcessState::BeginWaiting;
}

void MultiProcess::append_ring0_sync_request(MultiProcess::Ring0TaskCallback callback, void* data) {
	current_process->ring0_request.has_ring0_request = true;
	current_process->ring0_request.callback = callback;
	current_process->ring0_request.data = data;
}

void MultiProcess::end() {
	int exit_code;
	asm volatile("movl %%eax, %0" :"=b"(exit_code));
	MultiProcess::exit(current_process, exit_code);
	hang;
}

void _idle_code() {
	while (1);
}

static volatile u32 last_pid = 0;

MultiProcess::Process* MultiProcess::create(void *entry, const char *name) {
	Process* proc = (Process*) kmalloc(sizeof(Process), 0, 0);
	proc->page_dir = (MemoryManagement::PageDirectory*) kmalloc(sizeof(MemoryManagement::PageDirectory), 1, 0);
	proc->name = name;
	proc->wait_task.has_wait_task = false;
	proc->ring0_request.has_ring0_request = false;
	proc->state = MultiProcess::ProcessState::Runnable;

	proc->registers.ss = 0x20 | 0x03;
	proc->registers.eflags = 0x0202;
	proc->registers.cs = 0x18 | 0x03;
	proc->registers.eip = (u32) entry;

	proc->wait_task.registers.ss = 0x20 | 0x03;
	proc->wait_task.registers.eflags = 0x0202;
	proc->wait_task.registers.cs = 0x18 | 0x03;
	proc->wait_task.ebp = (u32) kmalloc(4 * KB, 0, 0);

	proc->pid = last_pid++;

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
	idle->state = MultiProcess::ProcessState::Idle;
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

MultiProcess::Process* MultiProcess::find_process_by_pid(u32 pid) {
	if (current_process->pid == pid) return current_process;

	MultiProcess::Process* proc = current_process->next;
	while (proc != current_process) {
		if (proc->pid == pid) return proc;
		proc = proc->next;
	}

	return 0;
}