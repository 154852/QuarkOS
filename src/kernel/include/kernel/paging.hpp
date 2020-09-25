#include <stddef.h>
#include <stdint2.h>

#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#define KB 1024
#define MB (KB * 1024)
#define PAGE_SIZE (4 * KB)

namespace MemoryManagement {
    struct PageDirectoryEntryFlags {
        u32 present : 1;
        u32 rw : 1;
        u32 user : 1;
        u32 write_through : 1;
        u32 cache_disabled : 1;
        u32 accessed : 1;
        u32 zero : 1;
        u32 page_size : 1;
        u32 ignored : 1;
        u32 avail : 3;
        u32 table_addr : 20;

        inline u32 raw() {
            return *(u32*) this;
        }
    };

    class PageDirectoryEntry {
        PageDirectoryEntryFlags* flags;
    public:
        explicit PageDirectoryEntry(PageDirectoryEntryFlags* _flags): flags(_flags) {}

        PageDirectoryEntryFlags& get_flags() {
            return *flags;
        }

        static PageDirectoryEntry from(u32* flags) {
            return PageDirectoryEntry((PageDirectoryEntryFlags*) flags);
        }
    };

    struct PageTableEntryFlags {
        u32 present : 1;
        u32 rw : 1;
        u32 user : 1;
        u32 write_through : 1;
        u32 cache_disabled : 1;
        u32 accessed : 1;
        u32 dirty : 1;
        u32 zero : 1;
        u32 global : 1;
        u32 avail : 3;
        u32 page_addr : 20;

        inline u32 raw() {
            return *(u32*) this;
        }
    };

    class PageTableEntry {
        PageTableEntryFlags* flags;
    public:
        explicit PageTableEntry(PageTableEntryFlags* _flags): flags(_flags) {}

        PageTableEntryFlags& get_flags() {
            return *flags;
        }

        static PageTableEntry from(u32* flags) {
            return PageTableEntry((PageTableEntryFlags*) flags);
        }
    };

    void init_paging();

    void protect_map(linear_addr_t, size_t length);
    void identity_map(linear_addr_t, size_t length);
    PageTableEntry ensure_pte(linear_addr_t);

    void allocate_physical_pages(physical_addr_t* addresses, size_t count);
};

#endif