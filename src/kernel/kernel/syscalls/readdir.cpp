#include "kernel/ustar.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <assertions.h>
#include <string.h>
#include <syscall.h>
#include <kernel/kmalloc.hpp>

void readdir_syscall_wait_task() {
    MultiProcess::Process* task = MultiProcess::get_current_task();

    const char* dirname = reinterpret_cast<const char*>(task->registers.ebx);
    size_t len = strlen(dirname);

    unsigned int* pointers = (unsigned int*) kmalloc(sizeof(unsigned int) * task->registers.edx);
    int count = USTAR::list_dir(dirname, pointers, task->registers.edx);

    if (count == -1) {
        task->registers.eax = -EFILENOTFOUND;
        task->state = MultiProcess::EndWaiting;
        task->wait_task.has_wait_task = false;
        yield();
        return;
    }

    DirEntry* entries = reinterpret_cast<DirEntry*>(task->registers.ecx);

    int lastentry = 0;
    for (int i = 0; i < count; i++) {
        USTAR::FileRaw* raw = USTAR::lookup_raw_from_raw_pointer(pointers[i]);
        memcpy(entries[lastentry].name, raw->name, 64);

        switch (raw->typeflag) {
            case FILE_TYPE_NORMAL: {
                entries[lastentry].type = FT_File;
                break;
            }
            case FILE_TYPE_DIRECTORY: {
                entries[lastentry].type = FT_Directory;
                break;
            }
            default: break;
        }

        lastentry++;
    }

    size_t length; Socket::Socket* sockets = Socket::all(&length);
    
    for (int i = 0; i < length; i++) {
        if (sockets[i].present) {
            char* name = sockets[i].name;
            if (!memcmp(name, dirname, len)) { // Note the lack of strlen + 1
                size_t filenamelen = strlen(name);

                for (int i = len; i < filenamelen; i++) {
                    if (name[i] == '/') {
                        goto entry_end;
                    }
                }

                memcpy(entries[lastentry].name, name, filenamelen);
                entries[lastentry].type = FT_Socket;
                lastentry++;

                entry_end:;
            }
        }
    }

    task->registers.eax = lastentry;

    task->state = MultiProcess::EndWaiting;
    task->wait_task.has_wait_task = false;
    yield();
}


void sys_readdir(IRQ::CSITRegisters2* frame) {
	MultiProcess::append_wait_task(readdir_syscall_wait_task);
	MultiProcess::yield(frame);
}