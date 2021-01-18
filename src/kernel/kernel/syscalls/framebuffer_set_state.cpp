#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/BXVGA.hpp>

#include <syscall.h>

void sys_framebuffer_set_state(IRQ::CSITRegisters2* frame) {
	FrameBufferInfo* info = reinterpret_cast<FrameBufferInfo*>(frame->ebx);
            
	if (info->width != 0 && info->height != 0)
		BXVGA::set_resolution(info->width, info->height);
	if ((bool) info->enabled != BXVGA::is_enabled()) {
		if (info->enabled) BXVGA::enable();
		else BXVGA::disable();
	}

	info->framebuffer = BXVGA::framebuffer();
	info->size = BXVGA::framebuffer_size();
	info->width = BXVGA::width();
	info->height = BXVGA::height();
	info->enabled = BXVGA::is_enabled();
}