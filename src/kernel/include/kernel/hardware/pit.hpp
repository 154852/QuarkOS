#include <stdint2.h>

#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#define PIT_CHANNEL_0 0
#define PIT_CHANNEL_1 1
#define PIT_CHANNEL_2 2

namespace PIT {
	void set_reload_value(unsigned short reload_value);
	bool can_have_frequency(unsigned int frequency);
	bool attempt_to_set_frequency(unsigned int frequency);

	void initialise_timer();
	void tick();
	u64 get_time();
	u64 fractional_time();
}

#endif