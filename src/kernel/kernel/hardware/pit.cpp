#include <kernel/hardware/pit.hpp>
#include <stdio.h>

#define BASE_FREQUENCY 1193182

void PIT::set_reload_value(unsigned char channel, unsigned short reload_value) {
	outb(0x43, 0b00110100 | ((u16) channel << 6));
	outb(0x40 + channel, reload_value);
	outb(0x40 + channel, reload_value >> 8);
}

bool PIT::can_have_frequency(unsigned int frequency) {
	return frequency < BASE_FREQUENCY;
}

bool PIT::attempt_to_set_frequency(unsigned int frequency) {
	if (!can_have_frequency(frequency)) return false;
	unsigned int reload_value = BASE_FREQUENCY / frequency;
	set_reload_value(PIT_CHANNEL_0, reload_value);
	return true;
}

// unsigned int PIT::get_reload_value_for(unsigned int ms) {
// 	// return ms * ((3579545 / 3) * 1000);
// 	// reload_value * PIT_HZ = ms
// 	return ms / PIT_HZ;
// }