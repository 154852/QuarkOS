#include <kernel/hardware/disk.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/kernel.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/kmalloc.hpp>
#include <assertions.h>

Disk::IDEDrive drive[4];
static volatile bool has_interrupted = false;
static volatile bool is_busy = false;

#define LSB(x) ((x) & 0xFF)
#define MSB(x) (((x) >> 8) & 0xFF)

#define IRQ_FIXED_DISK 14

inline void disable_disk_irq() {
	PIC::irq_set_mask(IRQ_FIXED_DISK);
}

inline void enable_disk_irq() {
	PIC::irq_clear_mask(IRQ_FIXED_DISK);
}

inline void await_interrupt() {
	assert(!is_busy);
	is_busy = true;
	while (!has_interrupted);
	is_busy = false;
}

void Disk::initialise() {
	has_interrupted = false;
	enable_disk_irq();

 	while (inb(IDE0_STATUS) & BUSY);

	outb(0x1F6, 0xA0);
	outb(0x3F6, 0xA0);
	outb(IDE0_COMMAND, IDENTIFY_DRIVE);

	enable_disk_irq();
	await_interrupt();

	void* wbuf = (void*) kmalloc(512);
	u8* byteBuffer = (u8*) kmalloc(512);
	u8* b = byteBuffer;
	u16* wbufbase = (u16*) wbuf;
	u16* w = (u16*) wbuf;

	for (u32 i = 0; i < 256; i++) {
		u16 data = inw(IDE0_DATA);
		*(w++) = data;
		*(b++) = MSB(data);
		*(b++) = LSB(data);
	}

	for (u32 i = 93; i > 54 && byteBuffer[i] == ' '; --i)
		byteBuffer[i] = 0;
	
	drive[0].cylinders = wbufbase[1];
	drive[0].heads = wbufbase[3];
	drive[0].sectors_per_track = wbufbase[6];
}

void Disk::read_sectors(u32 start, u16 count, u8* result) {
	assert(count <= 256);

	disable_disk_irq();

	while (inb(IDE0_STATUS) & BUSY);

	outb(0x1f2, count == 256? 0:LSB(count));
	outb(0x1f3, start & 0xff);
	outb(0x1f4, (start >> 8) & 0xff);
	outb(0x1f5, (start >> 16) & 0xff);
	outb(0x1f6, 0xe0 | ((start >> 24) & 0xf));

	outb(0x3f6, 0x08);
	while (!(inb(IDE0_STATUS) & DRDY));

	outb(IDE0_COMMAND, READ_SECTORS);

	for (int i = 0; i < count; i++) {
		has_interrupted = false;
		enable_disk_irq();
		await_interrupt();
		disable_disk_irq();

		u8 status = inb(0x1f7);
		assert(status & DRQ);

		inw_many(IDE0_DATA, result + (512 * i), 256);
	}
}

void Disk::read_sectors_limited(u32 start, u16 count, u8* result, u32 dstart, u32 dlength) {
	assert(count <= 256);

	disable_disk_irq();

	while (inb(IDE0_STATUS) & BUSY);

	outb(0x1f2, count == 256? 0:LSB(count));
	outb(0x1f3, start & 0xff);
	outb(0x1f4, (start >> 8) & 0xff);
	outb(0x1f5, (start >> 16) & 0xff);
	outb(0x1f6, 0xe0 | ((start >> 24) & 0xf));

	outb(0x3f6, 0x08);
	while (!(inb(IDE0_STATUS) & DRDY));

	outb(IDE0_COMMAND, READ_SECTORS);

	for (int i = 0; i < count; i++) {
		has_interrupted = false;
		enable_disk_irq();
		await_interrupt();
		disable_disk_irq();

		u8 status = inb(0x1f7);
		assert(status & DRQ);

		for (int j = 0; j < 512; j += 2) {
			u16 value = inw(IDE0_DATA);

			int idx = (i * 512) + j - dstart;
			if (idx >= 0 && idx < (int) dlength) result[idx] = LSB(value);
			idx++;
			if (idx >= 0 && idx < (int) dlength) result[idx] = MSB(value);
		}
	}
}

void Disk::disk_interrupt(void*) {
	PIC::send_EOI(IRQ_FIXED_DISK);
	char status = inb(0x1f7); // status
	if (status & 1) { // ERR
		u8 err = inb(0x1f1);
		kdebugf("[Disk] Disk Error: %d\n", err);
		hang;
	}
	has_interrupted = true;
}

void Disk::write_sectors(u32 start, u16 count, u8* data) {
	assert(count <= 256);

	disable_disk_irq();

	outb(0x1f2, count == 256? 0:LSB(count));
	outb(0x1f3, start & 0xff);
	outb(0x1f4, (start >> 8) & 0xff);
	outb(0x1f5, (start >> 16) & 0xff);
	outb(0x1f6, 0xe0 | ((start >> 24) & 0xf));
	
	outb(0x3F6, 0x08);

	outb(IDE0_COMMAND, WRITE_SECTORS);
	while (!(inb(IDE0_STATUS) & DRQ));

	u8 status = inb(0x1f7);
	assert(status & DRQ);

	for (int i = 0; i < count; i++) {
		outw_many(IDE0_DATA, data + (i * 512), 256);

		has_interrupted = false;
		enable_disk_irq();
		await_interrupt();
		disable_disk_irq();
	}

	outb(IDE0_COMMAND, FLUSH_CACHE);
	while (inb(IDE0_STATUS) & BUSY);
	has_interrupted = false;

	enable_disk_irq();
	await_interrupt();
}