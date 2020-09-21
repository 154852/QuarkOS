#include <stdint2.h>

#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#define PIT_CHANNEL_0 0
#define PIT_CHANNEL_1 1
#define PIT_CHANNEL_2 2

void pit_set_reload_value(unsigned char channel, unsigned short reload_value);
unsigned short pit_get_reload_value_for(unsigned int hz);

#endif