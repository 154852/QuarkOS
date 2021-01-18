#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/ustar.hpp>
#include <kernel/elf.hpp>

#include <syscall.h>

struct ELFLoadRequest {
    USTAR::FileParsed* file;
    MultiProcess::Process* proc;
};

void load_elf_ring0_callback(void* data) {
    ELFLoadRequest* req = static_cast<ELFLoadRequest*>(data);
    USTAR::FileParsed* file = req->file;

    MultiProcess::Process* proc = ELF::load_static_source(file->content, file->length, MultiProcess::create(0, file->name));
    req->proc = proc;

    MultiProcess::get_current_task()->ring0_request.has_ring0_request = false;
}

void execve_syscall_wait_task() {
    MultiProcess::Process* task = MultiProcess::get_current_task();

    USTAR::FileParsed* file = USTAR::lookup_parsed((const char*) task->registers.ebx);
    if (file == 0) {
        task->registers.eax = -EFILENOTFOUND;
        task->state = MultiProcess::EndWaiting;
        task->wait_task.has_wait_task = false;
        yield();
        return;
    }

    ELFLoadRequest req = {
        .file = file,
        .proc = 0
    };

    MultiProcess::append_ring0_sync_request(load_elf_ring0_callback, &req);
    yield();
    MultiProcess::append(req.proc);

    task->registers.eax = req.proc->pid;

    task->state = MultiProcess::EndWaiting;
    task->wait_task.has_wait_task = false;
    yield();
}

void sys_exec(IRQ::CSITRegisters2* frame) {
	MultiProcess::append_wait_task(execve_syscall_wait_task);
	MultiProcess::yield(frame);
}