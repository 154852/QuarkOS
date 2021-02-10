#include <stdint2.h>
#include <stdio.h>

#ifndef _KERNEL_DISK_H
#define _KERNEL_DISK_H

#define IDE0_DATA 0x1F0
#define IDE0_STATUS 0x1F7
#define IDE0_COMMAND 0x1F7
#define IDENTIFY_DRIVE 0xEC

#define BUSY (1 << 7)
#define DRDY (1 << 6)
#define DRQ (1 << 3)
#define READ_SECTORS 0x21
#define WRITE_SECTORS 0x30
#define FLUSH_CACHE 0xe7

#define IDE0_DISK0 0
#define IDE0_DISK1 1
#define IDE1_DISK0 2
#define IDE1_DISK1 3

namespace Disk {
    void initialise();
	void read_sectors(u32 start, u16 count, u8* result);
	void read_sectors_limited(u32 start, u16 count, u8* result, u32 dstart, u32 dlength);

	void write_sectors(u32 start, u16 count, u8* data);
	
	__attribute__((interrupt)) void disk_interrupt(void*);

	struct IDEDrive {
		u16 cylinders;
		u16 heads;
		u16 sectors_per_track;
	};
}

#endif