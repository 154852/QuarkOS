#include "kernel/ustar.hpp"
#include <kernel/syscall.hpp>
#include <kernel/socket.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/pic.hpp>
#include <assertions.h>
#include <string.h>
#include <syscall.h>
#include <kernel/kmalloc.hpp>

void sys_readdir(IRQ::CSITRegisters2* frame) {
    PIC::irq_set_mask(0);
    asm("sti");

    const char* dirname = reinterpret_cast<const char*>(frame->ebx);
    size_t len = strlen(dirname);

    unsigned int* pointers = (unsigned int*) kmalloc(sizeof(unsigned int) * frame->edx);
    int count = USTAR::list_dir(dirname, pointers, frame->edx);

    if (count == -1) {
        frame->eax = -EFILENOTFOUND;
        return;
    }

    DirEntry* entries = reinterpret_cast<DirEntry*>(frame->ecx);

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
    
    for (size_t i = 0; i < length; i++) {
        if (sockets[i].present) {
            char* name = sockets[i].name;
            if (!memcmp(name, dirname, len)) { // Note the lack of strlen + 1
                size_t filenamelen = strlen(name);

                for (size_t j = len; j < filenamelen; j++) {
                    if (name[j] == '/') {
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

    frame->eax = lastentry;

    asm("cli");
    PIC::irq_clear_mask(0);
}