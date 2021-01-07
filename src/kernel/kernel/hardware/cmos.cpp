#include <assertions.h>
#include <kernel/hardware/cmos.hpp>
#include <stdio.h>

u8 read(u8 reg) {
	outb(0x70, reg);
	return inb(0x71);
}

u32 status_b() {
	return read(0x0b);
}

bool update_in_progress() {
	return read(0x0a) & 0x80;
}

u32 bcd_to_binary(u8 value) {
	if (!(status_b() & 0x04)) return (value & 0x0F) + ((value >> 4) * 10);
	return value;
} 

u32 CMOS::read_seconds() {
	while (update_in_progress());
	return bcd_to_binary(read(0x00));
}

u32 CMOS::read_minutes() {
	while (update_in_progress());
	return bcd_to_binary(read(0x02));
}

u32 CMOS::read_hours() {
	while (update_in_progress());

	u32 hour = read(0x04);
	bool is_pm = hour & 0x80;
	hour = bcd_to_binary(hour & 0x7f);

	if (!(status_b() & 0x02)) {
		hour = hour % 12;
		if (is_pm) hour += 12;
	}

	return hour;
}

u32 CMOS::read_day_of_month() {
	while (update_in_progress());
	return bcd_to_binary(read(0x07));
}

u32 CMOS::read_month() {
	while (update_in_progress());
	return bcd_to_binary(read(0x08));
}

u32 CMOS::read_years() {
	while (update_in_progress());
	return bcd_to_binary(read(0x09)) + 2000;
}