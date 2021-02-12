#include <assertions.h>
#include <kernel/hardware/pit.hpp>
#include <kernel/kernel.hpp>
#include <kernel/hardware/cmos.hpp>
#include <stdio.h>

#define BASE_FREQUENCY 1193182

u32 frequency;
u64 time;
u64 ticks = 0;

#define TIMER0_SELECT 0
#define WRITE_WORD 0x30
#define MODE_COUNTDOWN 0
#define MODE_SQUARE_WAVE 0x06

void PIT::initialise_timer() {
	time = CMOS::secs_since_epoch();
	kdebugf("[PIT] Startup time = %d\n", time);
	outb(0x43, TIMER0_SELECT | WRITE_WORD | MODE_SQUARE_WAVE);
	attempt_to_set_frequency(250);
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

void PIT::set_reload_value(unsigned short reload_value) {
	frequency = BASE_FREQUENCY / reload_value;
	kdebugf("[PIT] Set clock frequency to %dHz (reload value = %d)\n", frequency, reload_value);

	outb(0x40, reload_value);
	outb(0x40, reload_value >> 8);
}

bool PIT::can_have_frequency(unsigned int frequency) {
	return frequency < BASE_FREQUENCY;
}

bool PIT::attempt_to_set_frequency(unsigned int frequency) {
	if (!can_have_frequency(frequency)) return false;
	unsigned int reload_value = BASE_FREQUENCY / frequency;
	assert(reload_value <= 0xffff);
	set_reload_value(reload_value);
	return true;
}