#include "kernel/kernel.hpp"
#include "syscall.h"
#include <kernel/syscall.hpp>
#include <kernel/hardware/pit.hpp>

void sys_getfulltime(IRQ::CSITRegisters2* frame) {
	FullTime* time = reinterpret_cast<FullTime*>(frame->ebx);
	time->year = CMOS::read_years();
	time->month = CMOS::read_month();
	time->date = CMOS::read_day_of_month();

	time->hour = CMOS::read_hours();
	time->minute = CMOS::read_minutes();
	time->second = CMOS::read_seconds();
	
	time->weekday = CMOS::read_day_of_week();
}