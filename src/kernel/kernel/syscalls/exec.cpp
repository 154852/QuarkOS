#include <ext2/init.hpp>
#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/pic.hpp>
#include <ext2/path.hpp>
#include <kernel/elf.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/kernel.hpp>

#include <syscall.h>
#include <string.h>

void sys_exec(IRQ::CSITRegisters2* frame) {
    ext2::INode* node = ext2::inode_from_root_path((const char*) frame->ebx);
    if (node == 0) {
        frame->eax = -EFILENOTFOUND;
        return;
    }

    int argc = frame->edx + 1;
    char** argvin = (char**) frame->ecx;
    char** argv = (char**) kmalloc(sizeof(char*) * argc);

    int len = strlen((char*) frame->ebx) + 1;
    argv[0] = (char*) kmalloc(len);
    memcpy(argv[0], (void*) frame->ebx, len);

    for (int i = 0; i < argc - 1; i++) {
        len = strlen(argvin[i]) + 1;
        char* arg = (char*) kmalloc(len);
        argv[i + 1] = arg;
        memcpy(arg, argvin[i], len);
    }

    MultiProcess::Process* proc = ELF::load_static_source((u8*) ext2::copy_file_to_kmem(node), node->size_low, MultiProcess::create(0, argv[0]), (const char**) argv, argc);
    MultiProcess::append(proc);

    frame->eax = proc->pid;
}