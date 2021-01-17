#include <kernel/hardware/interrupts.hpp>

#ifndef _KERNEL_MOUSE_H
#define _KERNEL_MOUSE_H

namespace Mouse {
	union MousePacketFlags {
		struct __attribute__((packed)) {
			unsigned char left_button : 1;
			unsigned char right_button : 1;
			unsigned char middle_button : 1;
			unsigned char always_1 : 1;
			unsigned char x_sign : 1;
			unsigned char y_sign : 1;
			unsigned char x_overflow : 1;
			unsigned char y_overflow : 1;
		};

		unsigned char raw;
	};

	struct MousePacket {
		MousePacketFlags flags;
		int x_delta;
		int y_delta;
		int scroll_delta_x;
		int scroll_delta_y;
	};

	void init();
	bool set_sample_rate(u8 sample_rate);
	
	void mouse_interrupt(struct IRQ::CSITRegisters*);
}

#endif