#include <kernel/hardware/pit.hpp>
#include <stdio.h>

#define PIT_HZ 1193182

void pit_set_reload_value(unsigned char channel, unsigned short reload_value) {
	outb(0x43, 0b00110100 | ((u16) channel << 6));
	outb(0x40 + channel, reload_value);
	outb(0x40 + channel, reload_value >> 8);
}

unsigned short pit_get_reload_value_for(unsigned int hz) {
	return hz / PIT_HZ;
}