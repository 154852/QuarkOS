#include <kernel/paging.h>
#include <kernel/kmalloc.h>
#include <string.h>
#include <stdio.h>

u32* frames;
u32 frame_count;

#define INDEX_FROM_BIT(a) (a / (8 * 4))
#define OFFSET_FROM_BIT(a) (a % (8 * 4))

/*
Enables the frame_addr'th bit in the bitset frames
*/
static void set_frame(u32 frame_addr) {
    u32 frame = frame_addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

/*
Disables the frame_addr'th bit in the bitset frames
*/
static void clear_frame(u32 frame_addr) {
    u32 frame = frame_addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

/*
Queries the frame_addr'th bit in the bitset frames
*/
static u32 test_frame(u32 frame_addr) {
    u32 frame = frame_addr / 0x1000;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

/*
Find the first unused frame
*/
static u32 first_free_frame() {
    for (u32 i = 0; i < INDEX_FROM_BIT(frame_count); i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (u32 j = 0; j < 32; j++) {
                u32 toTest = 0x1 << j;
                if (!(frames[i] & toTest)) return (i * 4 * 8) + j;
            }
        }
    }
    debugf("No free frame\n");
    return 0;
}

/*
Setup page, and store it in the frames bitset
*/
void alloc_frame(PageTableEntry* page, int is_kernel, int is_writable) {
    if (page->page_addr != 0) return;
    u32 idx = first_free_frame();
    set_frame(idx * 0x1000);
    page->present = 1;
    page->rw = is_writable? 1:0;
    page->user = is_kernel? 0:1;
    page->page_addr = idx;
}

/*
Remove the frame from the frames bitset
*/
void free_frame(PageTableEntry* page) {
    u32 frame;
    if (!(frame = page->page_addr)) return;
    clear_frame(frame);
    page->page_addr = 0;
}

static PageDirectory* kernel_directory;
static PageDirectory* current_directory;

/*
Get's the page holding address, if it doesn't exist, and make is enabled, create the page
*/
PageTableEntry* get_page(u32 address, int make, PageDirectory* directory) {
    address /= 0x1000;
    u32 table_idx = address / 1024;
    if (directory->tables_simple[table_idx]) return &directory->tables_simple[table_idx]->entries[address % 1024];
    if (make) {
        u32 tmp;
        directory->tables_simple[table_idx] = (PageTable*) kmalloc_ap(sizeof(PageTable), &tmp);
        memset(directory->tables_simple[table_idx], 0, 0x1000);
        // directory->entries[table_idx] = tmp | 0b111;
        directory->entries[table_idx].table_addr = tmp >> 12;
        directory->entries[table_idx].present = 1;
        directory->entries[table_idx].rw = 1;
        directory->entries[table_idx].user = 1;
        return &directory->tables_simple[table_idx]->entries[address % 1024];
    }
    return 0;
}

// void set_page_flags(u32 address, char present, char writable, char user_supervisor, PageDirectory* directory) {
//     address /= 0x1000;
//     u32 table_idx = address / 1024;

//     u32 flags = 0;
//     flags |= present;
//     flags |= writable << 1;
//     flags |= user_supervisor << 2;

//     directory->tables_physical[table_idx] = (directory->tables_physical[table_idx] & ~(0b111)) | flags;
// }

/*
Set's the active page directory to directory
*/
void switch_page_directory(PageDirectory* directory) {
    current_directory = directory;
    asm volatile("mov %0, %%cr3" :: "r"(&directory->entries));
    u32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

void init_paging() {
    u32 mem_end_page = 0x1000000;
    frame_count = mem_end_page / 0x1000;
    frames = (u32*) kmalloc_a(INDEX_FROM_BIT(frame_count));
    memset(frames, 0, INDEX_FROM_BIT(frame_count));

    kernel_directory = (PageDirectory*) kmalloc_a(sizeof(PageDirectory));
    memset(kernel_directory, 0, sizeof(PageDirectory));
    current_directory = kernel_directory;

    u32 i = 0;
    while ((void*) i < kmalloc_placement_address()) {
        alloc_frame(get_page((u32) i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    // alloc_frame(get_page(0, 1, kernel_directory), 0, 1);
    // set_page_flags(0, 0, 0, 0, kernel_directory);

    switch_page_directory(kernel_directory);
}

// static PageDirectory page_directory;

// void allocate_frame_for_page(PageTableEntry* entry) {
//     u32 unused = first_free_frame();
//     set_frame(unused * 0x1000);
//     entry->page_addr = unused; // no bitwise operations needed
// }

// void reset_pte_flags(PageTableEntry* entry, char present, char writable, char is_user) {
//     entry->present = present & 0x1;
//     entry->rw = writable & 0x1;
//     entry->user = is_user & 0x1;
//     entry->write_through = 0;
//     entry->cache_disabled = 0;
//     entry->accessed = 0;
//     entry->dirty = 0;
//     entry->zero = 0;
//     entry->global = 0;
//     entry->avail = 0;
// }

// PageTable* create_page_table_for(PageDirectory* page_directory, u32 index) {
//     u32 tmp;
//     PageTable* table = (PageTable*) kmalloc_ap(sizeof(PageTable), &tmp);

//     page_directory->tables_simple[index] = table;
//     memset(table, 0, 0x1000);
//     page_directory->entries[index].table_addr = tmp >> 12;
    
//     return table;
// }

// void reset_pde_flags(PageDirectoryEntry* entry, char present, char writable, char is_user) {
//     entry->present = present & 0x1;
//     entry->rw = writable & 0x1;
//     entry->user = is_user & 0x1;
//     entry->write_through = 0;
//     entry->cache_disabled = 0;
//     entry->accessed = 0;
//     entry->zero = 0;
//     entry->page_size = 0; // 4 KiB
//     entry->avail = 0;
// }

// void init_paging() {
//     memset(page_directory.entries, 0, 4096);

//     PageTable* entry = create_page_table_for(&page_directory, 0);
//     reset_pde_flags(&page_directory.entries[0], 0, 0, 0);
//     allocate_frame_for_page(&entry->entries[0]);

//     reset_pde_flags(&page_directory.entries[0], 1, 1, 1);
// }

/*
Pages allow for the creation of virtual address spaces, and protects programs from accessing one another's memory.

The CR3 register holds the current, active, page directory.
A page directory references 1024 page tables.
A page table references 1024 pages.

In the page table, virtual address = offset in page directory * 4096 + offset in page table.

A page is a section of memory in the virtual address space.
A frame is a section of memory in physical address space.

When we want a new page, we find a free frame, and point the page at that frame.
*/