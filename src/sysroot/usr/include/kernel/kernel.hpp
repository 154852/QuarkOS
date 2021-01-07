#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <stdio.h>
#include <kernel/hardware/cmos.hpp>

#define kdebugf(...) \
	debugf("[%.2d:%.2d:%.2d %.2d/%.2d/%.4d] ", CMOS::read_hours(), CMOS::read_minutes(), CMOS::read_seconds(), CMOS::read_day_of_month(), CMOS::read_month(), CMOS::read_years()); \
	debugf(__VA_ARGS__);

#endif