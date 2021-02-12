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

// #define HARDCODE_TIME

#ifdef HARDCODE_TIME
u32 CMOS::read_seconds() { return 0; }
u32 CMOS::read_minutes() { return 0; }
u32 CMOS::read_hours() { return 0; }
u32 CMOS::read_day_of_month() { return 2; }
u32 CMOS::read_month() { return 2; }
u32 CMOS::read_years() { return 2001; }
u32 CMOS::read_day_of_week() { return 2; }
#else
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
#endif

u32 days_in_month(u32 month, u32 year) {
	switch (month) {
	case 1: return 31;
	case 2: return year % 4 == 0? 29:28;
	case 3: return 31;
	case 4: return 30;
	case 5: return 31;
	case 6: return 30;
	case 7: return 31;
	case 8: return 31;
	case 9: return 30;
	case 10: return 31;
	case 11: return 30;
	case 12: return 31;
	}

	return 0;
}

u64 CMOS::secs_since_epoch() {
	int year = CMOS::read_years();
	int month = CMOS::read_month();
	u32 month_days = 0;
	for (int i = 1; i < month; i++) {
		month_days += days_in_month(i, year);
	}
	
	int year_days = 0;
	for (int i = 2000; i < year; i++) {
		for (int j = 1; j <= 12; j++) {
			year_days += days_in_month(j, i);
		}
	}

	return 
		CMOS::read_seconds() +
		(CMOS::read_minutes() * 60) + 
		(CMOS::read_hours() * 60 * 60) +
		((CMOS::read_day_of_month() - 1) * 24 * 60 * 60) + 
		(month_days * 24 * 60 * 60) + 
		(year_days * 24 * 60 * 60) + 946684800;
}