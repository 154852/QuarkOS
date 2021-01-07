#include <stdint2.h>

#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

namespace PIC {
	void send_EOI(unsigned char irq);
	void remap(unsigned char offset1, unsigned char offset2);
	void irq_set_mask(unsigned char IRQline);
	void irq_clear_mask(unsigned char IRQline);
	uint16_t get_irr(void);
	uint16_t get_isr(void);
};

#endif