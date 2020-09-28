#include <kernel/ustar.hpp>
#include <kernel/kmalloc.hpp>
#include <kernel/hardware/disk.hpp>
#include <string.h>

static unsigned char* open_archive;
static int open_archive_sector = -1;

void ensure_open_archive(int idx) {
    if (open_archive_sector == idx) return;
    if (open_archive_sector == -1) open_archive = (unsigned char*) kmalloc(512, 0, 0);
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
    u32 ptr = 0;

    while (!memcmp(archive_pointer(ptr + 257), "ustar", 5)) {
        int filesize = oct2bin(archive_pointer(ptr + 0x7c), 11);
        if (!memcmp(archive_pointer(ptr), filename, strlen(filename) + 1)) {
            return (USTAR::FileRaw*) archive_pointer(ptr);
        }
        ptr += (((filesize + 511) / 512) + 1) * 512;
    }
    return 0;
}

USTAR::FileParsed* USTAR::lookup_parsed(const char* filename) {
    FileRaw* raw = lookup_raw(filename);
    if (!raw) return 0;

    FileParsed* parsed = (FileParsed*) kmalloc(sizeof(FileParsed), 0, 0);
    size_t name_length = strlen(raw->name) + 1;
    parsed->name = (char*) kmalloc(name_length, 0, 0);
    memcpy(parsed->name, raw->name, name_length);

    parsed->length = oct2bin((unsigned char*) raw->size, 11);
    parsed->content = (unsigned char*) kmalloc(parsed->length + 1, 0, 0);
    memcpy(parsed->content, content_from_raw(raw), parsed->length);
    parsed->content[parsed->length] = 0;

    return parsed;
}

unsigned char* USTAR::content_from_raw(FileRaw* raw) {
    return archive_pointer(((unsigned int) raw - (unsigned int) open_archive + (512 * open_archive_sector)) + 512);
}

void USTAR::initialise() {
    // FileParsed* parsed = lookup_parsed("builtfs/hello.txt");
    // debugf("%s: (length = %i) '%s'\n", parsed->name, parsed->length, parsed->content);
}