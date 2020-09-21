#include <stdint2.h>

#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

void pic_send_EOI(unsigned char irq);
void pic_remap(unsigned char offset1, unsigned char offset2);
void pic_IRQ_set_mask(unsigned char IRQline);
void pic_IRQ_clear_mask(unsigned char IRQline);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif