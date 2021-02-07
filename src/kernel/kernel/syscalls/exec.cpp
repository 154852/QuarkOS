#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/ustar.hpp>
#include <kernel/elf.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/kernel.hpp>

#include <syscall.h>
#include <string.h>

void sys_exec(IRQ::CSITRegisters2* frame) {
    USTAR::FileParsed* file = USTAR::lookup_parsed((const char*) frame->ebx);
    if (file == 0) {
        frame->eax = -EFILENOTFOUND;
        return;
    }

    int argc = frame->edx + 1;
    char** argvin = (char**) frame->ecx;
    char** argv = (char**) kmalloc(sizeof(char*) * argc);
    argv[0] = file->name;
    for (int i = 0; i < argc - 1; i++) {
        char* arg = (char*) kmalloc(strlen(argvin[i]) + 1);
        argv[i + 1] = arg;
        memcpy(arg, argvin[i], strlen(argvin[i]) + 1);
    }
    MultiProcess::Process* proc = ELF::load_static_source(file->content, file->length, MultiProcess::create(0, file->name), (const char**) argv, argc);
    MultiProcess::append(proc);

    frame->eax = proc->pid;
}