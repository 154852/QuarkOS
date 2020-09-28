#include <stdint2.h>

#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

constexpr u8 ELF_MAGIC[4] = {0x7f, 'E', 'L', 'F'};

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

namespace ELF {
	struct __attribute__((packed)) Header {
		u8 magic[4];
		u8 bits; // ELF_*_BIT
		u8 endian; // ELF_*_ENDIAN
		u8 header_version;
		u8 os_abi;
		
		u8 padding[8];
		u8 type[2]; // ELF_RELOCATABLE ... ELF_CORE
		u8 instruction_set[2]; // ELF_INS_*
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

	void load_static_source(unsigned char* content, u32 length);
};

#endif