#include <kernel/syscall.hpp>
#include <kernel/multiprocess.hpp>
#include <kernel/hardware/BXVGA.hpp>

#include <syscall.h>

void sys_framebuffer_info(IRQ::CSITRegisters2* frame) {
	FrameBufferInfo* info = reinterpret_cast<FrameBufferInfo*>(frame->ebx);
	info->framebuffer = BXVGA::framebuffer();
	info->size = BXVGA::framebuffer_size();
	info->width = BXVGA::width();
	info->height = BXVGA::height();
	info->enabled = BXVGA::is_enabled();
}