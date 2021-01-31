#include <kernel/paging.hpp>
#include <kernel/kmalloc.hpp>
#include <string.h>
#include <assertions.h>
#include <stdio.h>
#include <kernel/kernel.hpp>

#define min(a, b) ((a) < (b)? (a):(b))

#define BLOCKS_PER_BUCKET 8

#define set_block_bit(bit) bitmap[bit / BLOCKS_PER_BUCKET] = bitmap[bit / BLOCKS_PER_BUCKET] | (1 << (bit % BLOCKS_PER_BUCKET))
#define clear_block_bit(i) bitmap[i / BLOCKS_PER_BUCKET] = bitmap[i / BLOCKS_PER_BUCKET] & (~(1 << (i % BLOCKS_PER_BUCKET)))
#define is_block_bit_set(i) ((bitmap[i / BLOCKS_PER_BUCKET] >> (i % BLOCKS_PER_BUCKET)) & 0x1)

#define pagedir_index(vaddr) (((u32) vaddr) >> 22)
#define pagetable_index(vaddr) ((((u32) vaddr) >> 12) & 0x3ff)
#define pageframe_index(vaddr) (((u32) vaddr) & 0xfff)

namespace MemoryManagement {
	PageDirectory* kernel_page_dir;
	PageDirectory* active_page_dir;
	bool paging_enabled = false;

	u32 total_blocks;
	u8* bitmap;
	u32 mem_start;

	void allocate_blocks(u32 block_count) {
		size_t length = block_count / 8;
		bitmap = (u8*) kmalloc(length);
		total_blocks = block_count;
		memset(bitmap, 0, length);
		mem_start = 16 * MB;
	}
	
	u32 first_free_block() {
		for (u32 i = 0; i < total_blocks; i++) {
			if (!is_block_bit_set(i)) return i;
		}
		assert(false);
		return -1;
	}

	u32 allocate_block() {
		u32 free_block = first_free_block();
		set_block_bit(free_block);
		return free_block;
	}

	void free_block(u32 idx) {
		clear_block_bit(idx);
	}

	void set_block_from_frame_addr(u32 addr) {
		clear_block_bit((addr - mem_start) / PAGE_SIZE);
	}

	void* virtual_to_physical(PageDirectory* dir, void* vaddr) {
		assert(paging_enabled);
		u32 page_dir_idx = pagedir_index(vaddr);
		u32 page_table_index = pagetable_index(vaddr);
		u32 page_frame_offset = pageframe_index(vaddr);
		PageTable* table = dir->ref_tables[page_dir_idx];
		assert(table);
		PageTableEntry entry = table->pages[page_table_index];
		assert(entry.present);
		return (void*) ((entry.page_addr << 12) + page_frame_offset);
	}

	// TODO: We do need to actually protect the kernel, but this makes syscalls a bit messy to handle
	PageTableEntry* allocate_page(PageDirectory* dir, u32 vaddr, u32 frame, bool __attribute__((unused)) is_kernel, bool __attribute__((unused)) is_writable, bool global) {
		assert(dir);

		u32 page_dir_idx = pagedir_index(vaddr);
		u32 page_tbl_idx = pagetable_index(vaddr);

		PageTable* table = dir->ref_tables[page_dir_idx];
		if (!table) {
			table = (PageTable*) kmalloc_aligned(sizeof(PageTable));
			memset(table, 0, sizeof(PageTable));

			dir->tables[page_dir_idx].table_addr = (u32) table >> 12;
			dir->tables[page_dir_idx].present = 1;
			dir->tables[page_dir_idx].rw = 1;
			dir->tables[page_dir_idx].user = 1;
			dir->ref_tables[page_dir_idx] = table;
		}

		if (!table->pages[page_tbl_idx].present) {
			table->pages[page_tbl_idx].page_addr = frame >> 12;
			table->pages[page_tbl_idx].present = 1;
			table->pages[page_tbl_idx].rw = is_writable? 1:0;
			table->pages[page_tbl_idx].user = is_kernel? 0:1;
			/*
				Avail bits: abc
				a: 1 if shared, 0 otherwise
				b: unused
				c: unused
			*/
			table->pages[page_tbl_idx].avail = global? 1:0;
		} else {
			// TODO: We ought to properly prevent remapping memory
			assert(false);
		}

		return &table->pages[page_tbl_idx];
	}

	void identity_map_region(PageDirectory* dir, u32 addr, size_t length, bool is_kernel, bool is_writable, bool global) {
		for (u32 i = 0; i < length; i += 4 * KB) {
			allocate_page(dir, addr + i, addr + i, is_kernel, is_writable, global);
		}
	}

	void enable_paging_bit() {
		asm volatile(
			"movl %%cr0, %%eax\n"
			"orl $0x80000001, %%eax\n"
			"movl %%eax, %%cr0\n"
			:::"eax", "memory"
		);
	}

	void save_kernel_page_dir() {
		load_page_dir(kernel_page_dir);
	}

	PageDirectory* get_kernel_page_dir() {
		return kernel_page_dir;
	}

	PageDirectory* get_active_page_dir() {
		return active_page_dir;
	}

	void load_page_dir(PageDirectory* dir) {
		asm volatile(
			"movl %%eax, %%cr3\n"
			::"a"(&dir->tables[0])
			:"memory"
		);
		active_page_dir = dir;
	}

	size_t allocate_region(PageDirectory* dir, u32 vaddr, u32 size, bool is_kernel, bool is_writable, bool global) {
		for (u32 i = 0; i < size; i += 4 * KB) {
			u32 block = allocate_block();
			u32 addr = (block * PAGE_SIZE) + mem_start;
			allocate_page(dir, vaddr + i, addr, is_kernel, is_writable, global);
		}

		return (size % PAGE_SIZE == 0)? (size / PAGE_SIZE):((size / PAGE_SIZE) + 1);
	}

	void free_pages(PageDirectory* dir) {
		assert(active_page_dir != dir && dir != kernel_page_dir);

		unsigned int freed = 0;
		for (int i = 0; i < 1024; i++) {
			if (!dir->tables[i].present) continue;

			for (int j = 0; j < 1024; j++) {
				PageTableEntry pte = dir->ref_tables[i]->pages[j];
				if (!pte.present || pte.avail & 0x01) continue;

				unsigned frame = pte.page_addr << 12;
				assert(is_block_bit_set((frame - mem_start) / PAGE_SIZE));
				clear_block_bit((frame - mem_start) / PAGE_SIZE);
				freed++;
			}
			dir->tables[i].present = 0;
		}

		char memsize[16];
		memset(memsize, 0, sizeof(memsize));
		mem_size_to_str(memsize, freed * PAGE_SIZE);
		kdebugf("[MemoryManagement] Freed page directory, clearing %d pages (%s)\n", freed, memsize);
	}

	void mem_size_to_str(char string[16], size_t size) {
		int number;
		const char* name;

		if (size < 1024) {
			number = size;
			name = "B";
		} else if (size < 1024 * 1024) {
			number = size / 1024;
			name = "KB";
		} else if (size < 1024 * 1024 * 1024) {
			number = size / 1024 / 1024;
			name = "MB";
		} else {
			number = size / 1024 / 1024 / 1024;
			name = "GB";
		}

		char numberstr[16];
		itoa(number, numberstr, 10);
		int nsl = strlen(numberstr);
		memcpy(string, numberstr, nsl);
		memcpy(string + nsl, name, strlen(name));
	}

	void init_paging() {
		kernel_page_dir = (PageDirectory*) kmalloc_aligned(sizeof(PageDirectory));
		memset(kernel_page_dir, 0, sizeof(PageDirectory));
		active_page_dir = kernel_page_dir;
		paging_enabled = true;

		allocate_blocks((GB >> 12) << 2); // 4GB: Just a big number

		identity_map_region(kernel_page_dir, 0, KERNEL_SIZE, true, true, true);

		load_page_dir(kernel_page_dir);
		enable_paging_bit();
	}
}