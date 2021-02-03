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

u32 CMOS::read_day_of_week() {
	while (update_in_progress());
	return bcd_to_binary(read(0x06));
}

// TODO: This is incredibly inaccurate
u64 CMOS::secs_since_epoch() {
	u32 year = CMOS::read_years();
	u64 time = (365 * year) + (year / 4) - (year / 100) + (year / 400);

	u32 month = CMOS::read_month();
	time += (30 * month) + (3 * (month + 1) / 5) + CMOS::read_day_of_month();

	time -= 719561;
	time *= 86400;
	
	time += (3600 * CMOS::read_hours()) + (60 * CMOS::read_minutes()) + CMOS::read_seconds();;
	return time;
}