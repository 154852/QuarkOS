#include <kernel/hardware/disk.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/kmalloc.hpp>

Disk::IDEDrive drive[4];
static volatile bool has_interrupted = false;

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
	while (!has_interrupted);
}

void Disk::initialise() {
	disable_disk_irq();
	has_interrupted = false;

 	while (inb(IDE0_STATUS) & BUSY);

	outb(0x1F6, 0xA0);
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

static Disk::CHS lba2chs(u8 drive_index, u16 lba) {
	Disk::IDEDrive d = drive[drive_index];
	Disk::CHS chs;
	chs.cylinder = lba / (d.sectors_per_track * d.heads);
    chs.head = (lba / d.sectors_per_track) % d.heads;
    chs.sector = (lba % d.sectors_per_track) + 1;
	return chs;
}

void Disk::read_sectors(u32 start, u16 count, u8* result) {
	disable_disk_irq();

	CHS chs = lba2chs(IDE0_DISK0, start);
	while (inb(IDE0_STATUS) & BUSY) {}

	outb(0x1F2, count == 256? 0:LSB(count));
	outb(0x1F3, chs.sector);
	outb(0x1F4, LSB(chs.cylinder));
	outb(0x1F5, MSB(chs.cylinder));
	outb(0x1F6, 0xA0 | chs.head);

	outb(0x3F6, 0x08);
	while (!(inb(IDE0_STATUS) & DRDY)) {}

	outb(IDE0_COMMAND, READ_SECTORS);
	has_interrupted = false;
	enable_disk_irq();
	await_interrupt();

	u8 status = inb(0x1f7);
	if (status & DRQ) {
		for (u32 i = 0; i < (count * 512); i += 2) {
			u16 w = inw(IDE0_DATA);
			result[i] = LSB(w);
			result[i + 1] = MSB(w);
		}
	}
}

void Disk::disk_interrupt(void*) {
	has_interrupted = true;
	inb(0x1f7); // status
	PIC::send_EOI(IRQ_FIXED_DISK);
}

// // Read queue?
// void Disk::async_read_sectors() {

// }