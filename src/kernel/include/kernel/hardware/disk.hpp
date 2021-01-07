#include <stdint2.h>
#include <stdio.h>

#ifndef _KERNEL_DISK_H
#define _KERNEL_DISK_H

#define IDE0_DATA 0x1F0
#define IDE0_STATUS 0x1F7
#define IDE0_COMMAND 0x1F7
#define BUSY 0x80
#define DRDY 0x40
#define DRQ 0x08
#define IDENTIFY_DRIVE 0xEC
#define READ_SECTORS 0x21

#define IDE0_DISK0 0
#define IDE0_DISK1 1
#define IDE1_DISK0 2
#define IDE1_DISK1 3

namespace Disk {
    void initialise();
	void read_sectors(u32 start, u16 count, u8* result);
	__attribute__((interrupt)) void disk_interrupt(void*);

	struct IDEDrive {
		u16 cylinders;
		u16 heads;
		u16 sectors_per_track;
	};

	struct CHS {
		u32 cylinder;
		u16 head;
		u16 sector;
	};
}

#endif