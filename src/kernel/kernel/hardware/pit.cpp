#include <kernel/hardware/pit.hpp>
#include <kernel/hardware/cmos.hpp>
#include <stdio.h>

#define BASE_FREQUENCY 1193182

u32 frequency;
u64 time;
u64 ticks = 0;

void PIT::initialise_timer() {
	time = CMOS::secs_since_epoch();
}

void PIT::tick() {
	ticks++;

	if (ticks == frequency) {
		ticks = 0;
		time++;
	}
}

u64 PIT::get_time() {
	return time;
}

void PIT::set_reload_value(unsigned char channel, unsigned short reload_value) {
	frequency = BASE_FREQUENCY / reload_value;

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