#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/ustar.hpp>
#include <kernel/elf.hpp>
#include <kernel/kernel.hpp>

#include <syscall.h>

void sys_exec(IRQ::CSITRegisters2* frame) {
    USTAR::FileParsed* file = USTAR::lookup_parsed((const char*) frame->ebx);
    if (file == 0) {
        frame->eax = -EFILENOTFOUND;
        return;
    }

    MultiProcess::Process* proc = ELF::load_static_source(file->content, file->length, MultiProcess::create(0, file->name));
    MultiProcess::append(proc);

    frame->eax = proc->pid;
}