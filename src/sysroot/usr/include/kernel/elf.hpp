#include <stdint2.h>
#include <kernel/multiprocess.hpp>

#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

constexpr u8 ELF_MAGIC[4] = {0x7f, 'E', 'L', 'F'};
constexpr u8 ELF_VERSION[4] = {0x01, 0x00, 0x00, 0x00};

#define ELF_32_BIT 1
#define ELF_64_BIT 2

#define ELF_LITTLE_ENDIAN 1
#define ELF_BIG_ENDIAN 2

#define ELF_RELOCATABLE 1
#define ELF_EXECUTABLE 2
#define ELF_SHARED 3
#define ELF_CORE 4

#define ELF_INS_NONSPECIFIC 0x0
#define ELF_INS_SPARC 0x2
#define ELF_INS_X86 0x3
#define ELF_INS_MIPS 0x8
#define ELF_INS_POWER_PC 0x14
#define ELF_INS_ARM 0x28
#define ELF_INS_SUPER_H 0x2A
#define ELF_INS_IA64 0x32
#define ELF_INS_X86_64 0x3E
#define ELF_INS_X86_A_ARCH_64 0xB7

#define ELF_SHT_NULL	  0		/* Section header table entry unused */
#define ELF_SHT_PROGBITS	  1		/* Program data */
#define ELF_SHT_SYMTAB	  2		/* Symbol table */
#define ELF_SHT_STRTAB	  3		/* String table */
#define ELF_SHT_RELA	  4		/* Relocation entries with addends */
#define ELF_SHT_HASH	  5		/* Symbol hash table */
#define ELF_SHT_DYNAMIC	  6		/* Dynamic linking information */
#define ELF_SHT_NOTE	  7		/* Notes */
#define ELF_SHT_NOBITS	  8		/* Program space with no data (bss) */
#define ELF_SHT_REL		  9		/* Relocation entries, no addends */
#define ELF_SHT_SHLIB	  10		/* Reserved */
#define ELF_SHT_DYNSYM	  11		/* Dynamic linker symbol table */
#define ELF_SHT_INIT_ARRAY	  14		/* Array of constructors */
#define ELF_SHT_FINI_ARRAY	  15		/* Array of destructors */
#define ELF_SHT_PREINIT_ARRAY 16		/* Array of pre-constructors */
#define ELF_SHT_GROUP	  17		/* Section group */
#define ELF_SHT_SYMTAB_SHNDX  18		/* Extended section indeces */
#define	ELF_SHT_NUM		  19		/* Number of defined types.  */
#define ELF_SHT_LOOS	  0x60000000	/* Start OS-specific.  */
#define ELF_SHT_GNU_HASH	  0x6ffffff6	/* GNU-style hash table.  */
#define ELF_SHT_GNU_LIBLIST	  0x6ffffff7	/* Prelink library list */
#define ELF_SHT_CHECKSUM	  0x6ffffff8	/* Checksum for DSO content.  */
#define ELF_SHT_LOSUNW	  0x6ffffffa	/* Sun-specific low bound.  */
#define ELF_SHT_SUNW_move	  0x6ffffffa
#define ELF_SHT_SUNW_COMDAT   0x6ffffffb
#define ELF_SHT_SUNW_syminfo  0x6ffffffc
#define ELF_SHT_GNU_verdef	  0x6ffffffd	/* Version definition section.  */
#define ELF_SHT_GNU_verneed	  0x6ffffffe	/* Version needs section.  */
#define ELF_SHT_GNU_versym	  0x6fffffff	/* Version symbol table.  */
#define ELF_SHT_HISUNW	  0x6fffffff	/* Sun-specific high bound.  */
#define ELF_SHT_HIOS	  0x6fffffff	/* End OS-specific type */
#define ELF_SHT_LOPROC	  0x70000000	/* Start of processor-specific */
#define ELF_SHT_HIPROC	  0x7fffffff	/* End of processor-specific */
#define ELF_SHT_LOUSER	  0x80000000	/* Start of application-specific */
#define ELF_SHT_HIUSER	  0x8fffffff	/* End of application-specific */

#define STACK_SIZE (8 * KB)

namespace ELF {
	struct __attribute__((packed)) Header {
		u8 magic[4];
		u8 bits; // ELF_*_BIT
		u8 endian; // ELF_*_ENDIAN
		u8 header_version;
		u8 os_abi;
		u8 padding[8];

		u8 type[2]; // ELF_RELOCATABLE ... ELF_CORE
		u8 instruction_set[2]; // ELF_INS_* / machine
		u8 version[4];
		u8 entry[4];
		u8 program_header_position[4];
		u8 section_header_position[4];
		u8 flags[4];
		u8 header_size[2];
		u8 program_header_entry_size[2];
		u8 program_header_entry_count[2];
		u8 section_header_entry_size[2];
		u8 section_header_entry_count[2];
		u8 section_header_names_index[2];
	};

	struct __attribute__((packed)) ProgramHeader {
		u8 segment_type[4];
		u8 offset[4];
		u8 virtual_memory_location[4];
		u8 undefined[4];
		u8 file_segment_size[4];
		u8 memory_segment_size[4];
		u8 flags[4];
		u8 alignment[4];
	};

	struct __attribute__((packed)) SectionHeader {
		u8 section_name[4]; // string tabel index
		u8 type[4];
		u8 flags[4];
		u8 addr[4];
		u8 offset[4];
		u8 size[4];
		u8 link[4];
		u8 info[4];
		u8 addralign[4];
		u8 entry_size[4];
	};

	MultiProcess::Process* load_static_source(unsigned char* content, u32 length, MultiProcess::Process* process);
};

#endif