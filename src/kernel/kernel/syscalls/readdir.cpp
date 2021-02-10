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
    while (dirname[len - 1] == '/') len--;

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
        memcpy(entries[lastentry].name, dirname, len);
        entries[lastentry].name[len] = '/';
        memcpy(entries[lastentry].name + len + 1, ent->name, ent->name_len_low);

        switch (ent->type_or_name_len_high) {
            case DIRENT_TYPE_FILE: {
                entries[lastentry].type = FT_File;
                break;
            }
            case DIRENT_TYPE_DIR: {
                entries[lastentry].type = FT_Directory;
                break;
            }
            case DIRENT_TYPE_SOCKET: {
                entries[lastentry].type = FT_Socket;
                break;
            }
            default: {
                entries[lastentry].type = FT_File;
                break;
            }
        }

        lastentry++;
    }

    frame->eax = lastentry;

    asm("cli");
    PIC::irq_clear_mask(0);
}