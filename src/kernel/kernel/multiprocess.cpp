#include "ext2/path.hpp"
#include <ext2/init.hpp>
#include <ext2/assist.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/kernel.hpp>
#include <assertions.h>
#include <kernel/kmalloc.hpp>
#include <kernel/hardware/gdt.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/hardware/pit.hpp>
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
	PIT::tick();
	if (current_process->ring == 0 && !current_process->is_kernel) {
		// TODO: We need to allow context switches here
		PIC::send_EOI(0);
		return 0;
	}

	MultiProcess::yield(registers);
	PIC::send_EOI(0);
	return 0;
}

void MultiProcess::yield(IRQ::CSITRegisters2* registers) {
	IRQ::CSITRegisters* original_registers = (IRQ::CSITRegisters*) ((unsigned char*) registers + 4);

	if (current_process->state != MultiProcess::ProcessState::Exitting) {
		memcpy(&current_process->registers, original_registers, sizeof(IRQ::CSITRegisters));
	}
	/* current_process = */ find_next_task();
	if (current_process->state == MultiProcess::ProcessState::Runnable) current_process->state = MultiProcess::ProcessState::Running;
	memcpy(original_registers, &current_process->registers, sizeof(IRQ::CSITRegisters));

	MemoryManagement::load_page_dir(current_process->page_dir);
}

asm (
	".globl context_switch_interrupt_trigger\n"
	"context_switch_interrupt_trigger:\n"
		"cli\n"
		"pushl %eax\n"
		"pushl %ebx\n"
		"pushl %ecx\n"
		"pushl %edx\n"
		"pushl %esi\n"
		"pushl %edi\n"
		"pushl %ebp\n"
        
		"movl $0x0, %eax\n"
        "mov %ds, %ax\n"
        "pushl %eax\n"
        
		"movl $0x0, %eax\n"
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
		
        "pushl %esp\n"
		"call update_process\n"
        "popl %esp\n"
        
        "popl %ebx\n"
        "mov %bx, %ds\n"
        "mov %bx, %es\n"
        "mov %bx, %fs\n"
        "mov %bx, %gs\n"
        
		"popl %ebp\n"
		"popl %edi\n"
		"popl %esi\n"
		"popl %edx\n"
		"popl %ecx\n"
		"popl %ebx\n"
		"popl %eax\n"
		"sti\n"
        
		"iret\n"
);

void _idle_code() {
	while (1);
}

static volatile u32 last_pid = 0;

unsigned status_socket_exec(void* id, void* data, unsigned length) {
	const char* str;
	unsigned len;

	switch (((MultiProcess::Process*) id)->state) {
		case MultiProcess::ProcessState::Runnable:
		case MultiProcess::ProcessState::Running: {
			str = "running";
			len = 7;
			break;
		};
		case MultiProcess::ProcessState::Exitting: {
			str = "exitting";
			len = 8;
			break;
		};
		case MultiProcess::ProcessState::Idle: {
			str = "idle";
			len = 4;
			break;
		};
		default: {
			str = "unknown";
			len = 7;
			break;
		}
	}

	len = len > length? length:len;
	memcpy(data, str, len);
	return len;
}

MultiProcess::Process* MultiProcess::create(void *entry, const char *name) {
	Process* proc = (Process*) kmalloc(sizeof(Process));
	proc->page_dir = (MemoryManagement::PageDirectory*) kmalloc_aligned(sizeof(MemoryManagement::PageDirectory));
	proc->name = name;
	proc->state = MultiProcess::ProcessState::Runnable;
	proc->ring = 3;

	proc->registers.ss = 0x20 | 0x03;
	// TODO: For now we will allow raised IOPL so we can use debugf from userspace programs
	proc->registers.eflags = 0x0202;
	proc->registers.cs = 0x18 | 0x03;
	proc->registers.eip = (u32) entry;

	proc->pid = last_pid++;

	char pidstr[16];
	itoa(proc->pid, pidstr, 10);

	ext2::mkdir(ext2::inode_id_from_root_path("/proc"), pidstr);
	char* path = ext2::path_join(2, "proc", pidstr);
	unsigned parent = ext2::inode_id_from_root_path(path);

	{
		ext2::mkdir(parent, "fd");
		char* fd = ext2::path_join(3, "proc", pidstr, "fd");
		unsigned fdparent = ext2::inode_id_from_root_path(fd);
		{
			proc->fd0 = Socket::new_socket();
			ext2::new_socket(fdparent, "0", proc->fd0->id);

			proc->fd1 = Socket::new_socket();
			ext2::new_socket(fdparent, "1", proc->fd1->id);
			
			proc->fd2 = Socket::new_socket();
			ext2::new_socket(fdparent, "2", proc->fd2->id);
		}

		ext2::new_text_file(parent, "exe", name);

		proc->status_sock = Socket::new_socket_with_gen(proc, status_socket_exec);
		ext2::new_socket(parent, "status", proc->status_sock->id);
	}
	
	return proc;
}

void MultiProcess::append(Process* proc) {
	proc->next = current_process->next;
	current_process->next = proc;
	kdebugf("[MultiProcess] Process %s (pid=%d) linked to process chain and will execute next\n", proc->name, proc->pid);
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

	current_process = (Process*) kmalloc(sizeof(Process));
	current_process->name = "kernel";
	current_process->state = ProcessState::Exitting;
	current_process->next = current_process;
	current_process->page_dir = MemoryManagement::get_kernel_page_dir();
	current_process->is_kernel = true;
	current_process->ring = 0;

	// MultiProcess::Process* idle = MultiProcess::create((void*) _idle_code, "idle");
	// idle->page_dir = MemoryManagement::get_kernel_page_dir();
	// idle->state = MultiProcess::ProcessState::Idle;
	// MultiProcess::append(idle);
}

void MultiProcess::tss_set_stack(u32 kss, u32 kesp) {
    kernel_tss.ss0 = kss;
    kernel_tss.esp0 = kesp;
}

void MultiProcess::exit(Process* process, u32 exit_code) {
	if (process == 0) process = current_process;
	
	process->state = ProcessState::Exitting;
	MemoryManagement::save_kernel_page_dir();
	MemoryManagement::free_pages(process->page_dir);
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