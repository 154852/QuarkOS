#include <stdint2.h>

#ifndef _KERNEL_PAGING_H
#define _KERNEL_PAGING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
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
} PageTableEntry;

typedef struct {
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
} PageDirectoryEntry;

typedef struct {
    PageTableEntry entries[1024];
} PageTable;

typedef struct {
    PageDirectoryEntry entries[1024];
    PageTable* tables_simple[1024];
} PageDirectory;

// typedef struct {
//     PageTable* tables[1024];
//     u32 tables_physical[1024];
//     u32 physical_addr;
// } PageDirectory;

void init_paging();

#ifdef __cplusplus
}
#endif

#endif