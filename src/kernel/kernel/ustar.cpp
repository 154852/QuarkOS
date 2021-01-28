#include <kernel/ustar.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/hardware/disk.hpp>
#include <string.h>

static unsigned char open_archive[512];;
static int open_archive_sector = -1;

void ensure_open_archive(int idx) {
    if (open_archive_sector == idx) return;
    open_archive_sector = idx;

    Disk::read_sectors(idx, 1, open_archive);
}

inline unsigned char* archive_pointer(unsigned int ptr) {
    ensure_open_archive(ptr / 512);
    return (open_archive - (open_archive_sector * 512)) + ptr;
}

int oct2bin(unsigned char *str, int size) {
    int n = 0;
    unsigned char *c = str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

USTAR::FileRaw* USTAR::lookup_raw(const char* filename) {
    unsigned int addr = USTAR::lookup_raw_pointer(filename);
    if (addr == 0) return 0;
    return (USTAR::FileRaw*) archive_pointer(addr);
}

unsigned int USTAR::lookup_raw_pointer(const char* filename) {
    unsigned int ptr = 0;

    if (filename[0] == '/') filename++;
    while (!memcmp(archive_pointer(ptr + 257), "ustar", 5)) {
        int filesize = oct2bin(archive_pointer(ptr + 0x7c), 11);
        if (!memcmp(archive_pointer(ptr), filename, strlen(filename) + 1)) {
            return ptr;
        }
        ptr += (((filesize + 511) / 512) + 1) * 512;
    }
    return 0;
}

#define min(a, b) ((a) > (b)? (b):(a))

USTAR::FileParsed* USTAR::lookup_parsed(const char* filename) {
    unsigned int raw_address = USTAR::lookup_raw_pointer(filename);
    if (!raw_address) return 0;

    USTAR::FileRaw* tmp_raw = (USTAR::FileRaw*) archive_pointer(raw_address);

    FileParsed* parsed = (FileParsed*) kmalloc(sizeof(FileParsed));
    size_t name_length = strlen(tmp_raw->name) + 1;
    parsed->name = (char*) kmalloc(name_length);
    memcpy(parsed->name, tmp_raw->name, name_length);

    parsed->length = oct2bin((unsigned char*) tmp_raw->size, 11);
    parsed->content = (unsigned char*) kmalloc(parsed->length + 1);
    unsigned int raw_content_start = raw_address + 512;
    for (u32 i = 0; i < parsed->length; i += 512) {
        memcpy(parsed->content + i, archive_pointer(raw_content_start + i), min(512, parsed->length - i));
    }

    parsed->content[parsed->length] = 0;

    return parsed;
}

USTAR::FileParsed* USTAR::lookup_parsed_from_raw_pointer(unsigned int raw_address) {
    USTAR::FileRaw* tmp_raw = (USTAR::FileRaw*) archive_pointer(raw_address);

    FileParsed* parsed = (FileParsed*) kmalloc(sizeof(FileParsed));
    size_t name_length = strlen(tmp_raw->name) + 1;
    parsed->name = (char*) kmalloc(name_length);
    memcpy(parsed->name, tmp_raw->name, name_length);

    parsed->length = oct2bin((unsigned char*) tmp_raw->size, 11);
    parsed->content = (unsigned char*) kmalloc(parsed->length + 1);
    unsigned int raw_content_start = raw_address + 512;
    for (u32 i = 0; i < parsed->length; i += 512) {
        memcpy(parsed->content + i, archive_pointer(raw_content_start + i), min(512, parsed->length - i));
    }

    parsed->content[parsed->length] = 0;

    return parsed;
}

USTAR::FileRaw* USTAR::lookup_raw_from_raw_pointer(unsigned int raw_address) {
    return (USTAR::FileRaw*) archive_pointer(raw_address);
}

int USTAR::list_dir(const char* dirname, unsigned int* pointers, size_t capacity) {
    unsigned int ptr = 0;
    unsigned int used = 0;

    int exists = 0;

    size_t len = strlen(dirname);
    if (dirname[0] == '/') {
        len--;
        dirname++;
    }

    while (!memcmp(archive_pointer(ptr + 257), "ustar", 5)) {
        USTAR::FileRaw* raw = (USTAR::FileRaw*) archive_pointer(ptr);
        int filesize = oct2bin((unsigned char*) raw->size, 11);

        if (!memcmp(raw->name, dirname, len)) { // Note the lack of strlen + 1
            size_t filenamelen = strlen(raw->name);
            if (len == filenamelen || (raw->name[0] == '/' && filenamelen == 1 && len == 0)) {
                exists = true;
                goto entry_end;
            }

            int searchend = filenamelen;
            if (raw->typeflag == FILE_TYPE_DIRECTORY) searchend--;
            
            for (int i = len; i < searchend; i++) {
                if (raw->name[i] == '/') {
                    goto entry_end;
                }
            }

            pointers[used++] = ptr;
        }

        entry_end:
        ptr += (((filesize + 511) / 512) + 1) * 512;
    }

    return exists? used:-1;
}