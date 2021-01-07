#include <stdint2.h>

#ifndef _KERNEL_CMOS_H
#define _KERNEL_CMOS_H

namespace CMOS {
	u32 read_seconds();
	u32 read_minutes();
	u32 read_hours();
	
	u32 read_day_of_month();
	u32 read_month();
	u32 read_years();
}

#endif