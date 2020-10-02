#include <stdint2.h>
#include <stdbool.h>
#include <kernel/hardware/interrupts.hpp>
#include <kernel/hardware/keyboard_scancodes.hpp>

#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

namespace Keyboard {
	struct KeyboardState {
		bool is_shifted;
		bool is_caps_locked;
	};

	char scan_code_to_char(const ScanCode* scancode, KeyboardState* state);
	__attribute__((interrupt)) void keyboard_interrupt(struct IRQ::CSITRegisters*);
}

#endif