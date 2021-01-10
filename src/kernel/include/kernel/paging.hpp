#include <stddef.h>
#include <stdint2.h>

#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#define KB 1024
#define MB (KB * 1024)
#define GB (MB * 1024)
#define PAGE_SIZE (4 * KB)

namespace MemoryManagement {
    struct __attribute__((packed)) PageDirectoryEntry {
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
    };

    struct __attribute__((packed)) PageTableEntry {
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
    };

    struct __attribute__((packed)) PageTable {
        PageTableEntry pages[1024];
    };

    struct __attribute__((packed)) PageDirectory {
        PageDirectoryEntry tables[1024];
        PageTable* ref_tables[1024];
    };

    PageTableEntry* allocate_page(PageDirectory* dir, u32 vaddr, u32 frame, bool is_kernel, bool is_writable);
    void identity_map_region(PageDirectory* dir, u32 addr, size_t length, bool is_kernel, bool is_writable);
    void allocate_region(PageDirectory* dir, u32 vaddr, u32 size, bool is_kernel, bool is_writable);
    void init_paging();
    void load_page_dir(PageDirectory* dir);
    void save_kernel_page_dir();
    
    PageDirectory* get_active_page_dir();
    PageDirectory* get_kernel_page_dir();
};

#endif