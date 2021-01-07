#ifndef _KERNEL_USTAR_H
#define _KERNEL_USTAR_H
 
#include <stddef.h>

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
	unsigned int lookup_raw_pointer(const char* name);
	FileParsed* lookup_parsed(const char* name);
};

#endif