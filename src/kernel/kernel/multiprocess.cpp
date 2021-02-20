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

#define KERNEL_STACK_SIZE (16 * KB)

void find_next_task() {
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

	if (current_process->state == MultiProcess::ProcessState::Runnable) current_process->state = MultiProcess::ProcessState::Running;
}

void MultiProcess::yield(IRQ::CSITRegisters2* registers) {
	assert(false);
}

struct x86_regs {
    u32
    edi, esi, ebp, ebx, ecx, edx, eax,
    eip, cs, eflags, esp, ss;
};

extern "C" void _multiprocess_tick(x86_regs* regs);
asm (
	".globl context_switch_interrupt_trigger\n"
	"context_switch_interrupt_trigger:\n"
		"cli\n"

		"push %eax\n"
		"push %edx\n"
		"push %ecx\n"
		"push %ebx\n"
		"push %ebp\n"
		"push %esi\n"
		"push %edi\n"

		"push %esp\n"
		"call _multiprocess_tick\n"
		"pop %eax\n"
		
		"pop %edi\n"
		"pop %esi\n"
		"pop %ebp\n"
		"pop %ebx\n"
		"pop %ecx\n"
		"pop %edx\n"
		"pop %eax\n"

		"sti\n"
        
		"iret\n"
);

extern "C" u32 read_eip();
asm (
	".globl read_eip\n"
	"read_eip:\n"
		"mov (%esp), %eax\n"
		"ret\n"
);

extern "C" void x86_goto(u32 eip, u32 ebp, u32 esp);
asm (
	".global x86_goto\n"
	"x86_goto:\n"
	    "pop %ebx\n"    /* Caller return address */
	    "pop %ebx\n"    /* eip */
	    "pop %ebp\n"
	    "pop %esp\n"
	    "mov $-1, %eax\n" /* Return -1 -> Done switching */
	    "jmp *%ebx\n"
);

extern "C" void x86_jump_user(u32 eax, u32 eip, u32 cs, u32 eflags, u32 esp, u32 ss);
asm (
	".global x86_jump_user\n"
	"x86_jump_user:\n"  /* eax, eip, cs, eflags, esp, ss */
	    "pop  %eax\n"   /* Caller return address */
	    "mov  $0x20 | 0x3, %ax\n"
	    "movw %ax, %ds\n"
	    "movw %ax, %es\n"
	    "movw %ax, %fs\n"
	    "movw %ax, %gs\n"
	    "pop  %eax\n"   /* eax for sys_fork return */
	    "iret\n"
);

static void thread_switch() {
    MemoryManagement::load_page_dir(current_process->page_dir);

    kernel_tss.esp0 = (u32) current_process->kernel_esp;

    x86_goto(current_process->eip, current_process->ebp, current_process->esp);
}

#define X86_SS (0x20 | 3)
#define X86_CS (0x18 | 3)

static void thread_spawn() {
	MemoryManagement::load_page_dir(current_process->page_dir);
	kernel_tss.esp0 = (u32) current_process->kernel_esp;
	current_process->spawned = true;

    x86_jump_user(current_process->registers.eax, current_process->registers.eip, X86_CS, current_process->registers.eflags, current_process->registers.esp, X86_SS);
}

static void schedule() {
	find_next_task();

    if (current_process->spawned) {
        thread_switch();
    } else {
        thread_spawn();
    }
}

void _multiprocess_tick(x86_regs* regs) {
	PIC::send_EOI(0);
	PIT::tick();

	volatile unsigned eip;
	volatile unsigned esp;
	volatile unsigned ebp;

	asm volatile("mov %%esp, %0":"=r"(esp));
    asm volatile("mov %%ebp, %0":"=r"(ebp));

    eip = read_eip();

    if (eip == (unsigned) -1) {
    	return;
    }

    current_process->eip = eip;
    current_process->esp = esp;
    current_process->ebp = ebp;

    schedule();
}

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
	memset(proc->page_dir, 0, sizeof(MemoryManagement::PageDirectory));
	proc->name = name;
	proc->state = MultiProcess::ProcessState::Runnable;
	proc->ring = 3;

	proc->kernel_esp = (u8*) kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	proc->spawned = false;

	proc->registers.ss = 0x20 | 0x03;
	// TODO: For now we will allow raised IOPL so we can use debugf from userspace programs
	proc->registers.eflags = 0x0202;
	proc->registers.cs = 0x18 | 0x03;
	proc->registers.eip = (u32) entry;

	proc->esp = 0;

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
	kdebugf("[MultiProcess] Process %s (pid=%d) linked to process chain and will execute next (esp0=%.8x)\n", proc->name, proc->pid, proc->kernel_esp);
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

	// TODO: rm /proc/pid entry and close all sockets
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