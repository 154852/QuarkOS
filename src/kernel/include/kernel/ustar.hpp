#ifndef _KERNEL_USTAR_H
#define _KERNEL_USTAR_H
 
#include <stddef.h>

#define FILE_TYPE_NORMAL '0'
#define FILE_TYPE_HARD_LINK '1'
#define FILE_TYPE_SYMBOLIC_LINK '2'
#define FILE_TYPE_CHARACTER_DEVICE '3'
#define FILE_TYPE_BLOCK_DEVICE '4'
#define FILE_TYPE_DIRECTORY '5'
#define FILE_TYPE_NAMED_PIPE '6'

namespace USTAR {
	struct __attribute__((packed)) FileRaw {
		char name[100];
		char mode[8];
		char uid[8];
		char gid[8];
		char size[12];
		char mtime[12];
		char chksum[8];
		char typeflag;
		char linkname[100];
		char magic[6];
		char version[2];
		char uname[32];
		char gname[32];
		char devmajor[8];
		char devminor[8];
		char prefix[155];
	};

	struct FileParsed {
		char* name;
		unsigned int length; // DOES NOT INCLUDE NULL POINTER
		unsigned char* content;
	};

	FileRaw* lookup_raw(const char* name);
	FileRaw* lookup_raw_from_raw_pointer(unsigned int raw_address);
	unsigned int lookup_raw_pointer(const char* name);
	FileParsed* lookup_parsed(const char* name);
	FileParsed* lookup_parsed_from_raw_pointer(unsigned int raw_address);
	int list_dir(const char* dirname, unsigned int* pointers, size_t capacity);
};

#endif