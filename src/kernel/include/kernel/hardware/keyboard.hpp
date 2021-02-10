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
		bool is_ctrl;
	};

	__attribute__((interrupt)) void keyboard_interrupt(struct IRQ::CSITRegisters*);
	
	void pop_from_buffer(u32 count);
	void push_to_buffer(char character);
	u32 get_buffer_size();
	char* get_buffer();
	void init_socket();
}

#endif