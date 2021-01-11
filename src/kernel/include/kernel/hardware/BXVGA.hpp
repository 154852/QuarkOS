#include <stdint2.h>

#ifndef _KERNEL_BXVGA_H
#define _KERNEL_BXGVA_H

namespace BXVGA {
	void initialise();

	void set_register(u16 index, u16 data);
	u16 get_register(u16 index);
	
	void set_resolution(u16 width, u16 height);
	
	void enable();
	void disable();
	bool is_enabled();

	u32* framebuffer();
	u32 framebuffer_size();
	u32 row_size();

	u16 width();
	u16 height();
};

#endif