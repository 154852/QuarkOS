#include <stdint2.h>
#include <stdbool.h>
#include <kernel/hardware/interrupts.h>
#include <kernel/hardware/keyboard_scancodes.h>

#ifndef _KERNEL_KEYBOARD_H
#define _KERNEL_KEYBOARD_H

typedef struct {
	bool is_shifted;
	bool is_caps_locked;
} KeyboardState;

static KeyboardState global_keyboard_state = { false };

char scan_code_to_char(const ScanCode* scancode, KeyboardState* state);
__attribute__((interrupt)) void keyboard_interrupt(struct interrupt_frame* frame);

#endif