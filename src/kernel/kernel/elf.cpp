#include <assertions.h>
#include <kernel/elf.hpp>
#include <stdio.h>
#include <string.h>

void ELF::load_static_source(unsigned char* content, u32 length) {
	Header* header = (Header*) content;

	assert(header->endian == ELF_LITTLE_ENDIAN);
	assert(memcmp(header->magic, ELF_MAGIC, 4) == 0);

	printf("Assertion passed, file loaded\n");
}