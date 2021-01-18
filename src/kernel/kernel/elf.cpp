#include "kernel/kernel.hpp"
#include <assertions.h>
#include <kernel/elf.hpp>
#include <kernel/hardware/BXVGA.hpp>
#include <kernel/paging.hpp>
#include <stdio.h>
#include <kernel/kmalloc.hpp>
#include <string.h>

u32 get_le_u32(const u8* le) {
	return (u32) le[0] | ((u32) le[1] << 8) | ((u32) le[2] << 16) | ((u32) le[3] << 24);
}

u16 get_le_u16(const u8* le) {
	return (u16) le[0] | ((u16) le[1] << 8);
}

MultiProcess::Process* ELF::load_static_source(unsigned char* content, u32 length, MultiProcess::Process* process) {
	MemoryManagement::PageDirectory* old = MemoryManagement::get_active_page_dir();
	MemoryManagement::save_kernel_page_dir();

	assert(length > sizeof(Header));
	Header* header = (Header*) content;

	assert(memcmp(header->magic, ELF_MAGIC, 4) == 0);
	assert(header->bits == ELF_32_BIT);
	assert(header->endian == ELF_LITTLE_ENDIAN);
	assert(header->header_version == 1);
	assert(header->instruction_set[0] == ELF_INS_X86);
	assert(memcmp(header->version, ELF_VERSION, 4) == 0);
	assert(header->type[0] == 0x02); // executable

	if (process == nullptr) process = MultiProcess::create(0, "<unnamed elf loaded>");

	MemoryManagement::identity_map_region(process->page_dir, 0, KERNEL_SIZE, true, true);

	u32 entry = get_le_u32(header->entry);

	u32 program_header_entry_size = get_le_u16(header->program_header_entry_size);
	assert(program_header_entry_size == sizeof(ProgramHeader));
	u32 program_header_entry_count = get_le_u16(header->program_header_entry_count);
	ProgramHeader* program_headers = (ProgramHeader*) ((u32) content + get_le_u32(header->program_header_position));

	for (u32 i = 0; i < program_header_entry_count; i++) {
		ProgramHeader pheader = program_headers[i];

		u32 flags = get_le_u32(pheader.flags);

		u32 begin = get_le_u32(pheader.virtual_memory_location);
		u32 size = get_le_u32(pheader.memory_segment_size);
		u32 fsize = get_le_u32(pheader.file_segment_size);
		u32 fbegin = get_le_u32(pheader.offset);

		assert((u32) &process < KERNEL_SIZE);
		assert(begin >= KERNEL_SIZE);
		MemoryManagement::allocate_region(process->page_dir, begin, size, false, true);

		MemoryManagement::load_page_dir(process->page_dir);
		memcpy((void*) begin, (const void*) (content + fbegin), (size_t) fsize);
		MemoryManagement::save_kernel_page_dir();

		if (flags & 1) {
			process->registers.eip = entry;
		}
	}

	const int EBP = 0xC0000000;
	MemoryManagement::allocate_region(process->page_dir, EBP, STACK_SIZE, false, true);

    process->registers.ebp = EBP;
	process->registers.esp = EBP + STACK_SIZE;
	process->registers.esp -= 4; // leave 0 on the stack
	MemoryManagement::load_page_dir(process->page_dir);
	(*(u32*) process->registers.esp) = (u32) MultiProcess::end;
	MemoryManagement::save_kernel_page_dir();

	MemoryManagement::load_page_dir(old);

	MemoryManagement::identity_map_region(process->page_dir, (u32) BXVGA::framebuffer(), BXVGA::framebuffer_size(), false, true);

	return process;
}