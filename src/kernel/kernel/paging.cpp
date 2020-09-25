#include <kernel/paging.hpp>
#include <kernel/kmalloc.h>
#include <string.h>
#include <stdio.h>

#define min(a, b) (a < b? a:b)

#define FREE_PAGE_COUNT ((16 * MB) - ((4 * MB) + 1024) + PAGE_SIZE - 1) / PAGE_SIZE

namespace MemoryManagement {
	static u32* page_directory;
	static u32* page_table_zero;
	static u32* page_table_one;
	static u32 free_pages[FREE_PAGE_COUNT];

  	void init_paging() {
		page_directory = (u32*) 0x5000;
		page_table_zero = (u32*) 0x6000;
		page_table_one = (u32*) 0x7000;

		memset(page_table_zero, 0, 4096);
		memset(page_table_one, 0, 4096);
		memset(page_directory, 0, 4096);

		protect_map(0, 4 * KB);
		identity_map(4096, 4 * MB);

		for (size_t i = (4 * MB) + 1024, j = 0; i < (16 * MB); i += PAGE_SIZE, j++) free_pages[j] = i;
		asm volatile("movl %%eax, %%cr3"::"a"(page_directory));
		asm volatile(
			"movl %cr0, %eax\n"
			"orl $0x80000001, %eax\n"
			"movl %eax, %cr0\n"
		);
	}

    void protect_map(linear_addr_t base_addr, size_t length) {
		for (u32 i = 0; i < length; i += 4096) {
			u32 addr = base_addr + i;

			PageTableEntry entry = ensure_pte(addr);
			entry.get_flags().page_addr = addr >> 12;
			entry.get_flags().user = 0;
			entry.get_flags().present = 0;
			entry.get_flags().rw = 0;
		}
	}

    void identity_map(linear_addr_t base_addr, size_t length) {
		for (u32 i = 0; i < length; i += 4096) {
			u32 addr = base_addr + i;

			PageTableEntry entry = ensure_pte(addr);
			entry.get_flags().page_addr = addr >> 12;
			entry.get_flags().user = 1;
			entry.get_flags().present = 1;
			entry.get_flags().rw = 1;
		}
	}

    PageTableEntry ensure_pte(linear_addr_t address) {
		u32 page_directory_index = (address >> 22) & 0x3ff;
		u32 page_table_index = (address >> 12) & 0x3ff;

		PageDirectoryEntry pde = PageDirectoryEntry::from(&page_directory[page_directory_index]);
		u32 table_addr = 0;
		if (!pde.get_flags().present) {
			if (page_directory_index == 0) {
				table_addr = reinterpret_cast<uintptr_t>(page_table_zero);
			} else if (page_directory_index == 1) {
				table_addr = reinterpret_cast<uintptr_t>(page_table_one);
			} else {
				debugf("Unknown PDI\n");
				while (1) {};
			}

			pde.get_flags().table_addr = table_addr >> 12;
			pde.get_flags().user = 1;
			pde.get_flags().present = 1;
			pde.get_flags().rw = 1;
		} else {
			table_addr = pde.get_flags().table_addr << 12;
		}

		return PageTableEntry::from(&((u32*) table_addr)[page_table_index]);
	}

    void allocate_physical_pages(physical_addr_t* addresses, size_t count) {
		int j = 0;
		for (int i = FREE_PAGE_COUNT - 1; i >= 0; i--) {
			if (free_pages[i]) {
				addresses[j++] = free_pages[i];
				free_pages[i] = 0;
				if (j == count) break;
			}
		}
	}
};