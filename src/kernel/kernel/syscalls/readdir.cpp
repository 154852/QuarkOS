#include "ext2/init.hpp"
#include "ext2/path.hpp"
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

    ext2::INode* dir = ext2::inode_from_root_path(dirname);

    if (dir == 0 || !ext2::is_dir(dir)) {
        frame->eax = -EFILENOTFOUND;
        return;
    }

    DirEntry* entries = reinterpret_cast<DirEntry*>(frame->ecx);

    int count = ext2::dir_entry_count(dir);

    int lastentry = 0;
    for (int i = 0; i < count; i++) {
        ext2::DirectoryEntry* ent = ext2::read_tmp_direntry(dir, i);
        if (strcmp(ent->name, ".") == 0 || strcmp(ent->name, "..") == 0) continue;

        memset(entries[lastentry].name, 0, 64);
        memcpy(entries[lastentry].name, ent->name, ent->name_len_low);

        switch (ent->type_or_name_len_high) {
            case DIRENT_TYPE_FILE: {
                entries[lastentry].type = FT_File;
                break;
            }
            case DIRENT_TYPE_DIR: {
                entries[lastentry].type = FT_Directory;
                break;
            }
            default: {
                entries[lastentry].type = FT_File;
                break;
            }
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