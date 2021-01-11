#include <kernel/hardware/BXVGA.hpp>
#include <kernel/hardware/pci.hpp>
#include <assertions.h>
#include <kernel/kernel.hpp>
#include <stdio.h>
#include <string.h>

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID 0x0
#define VBE_DISPI_INDEX_XRES 0x1
#define VBE_DISPI_INDEX_YRES 0x2
#define VBE_DISPI_INDEX_BPP 0x3
#define VBE_DISPI_INDEX_ENABLE 0x4
#define VBE_DISPI_INDEX_BANK 0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH 0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET 0x8
#define VBE_DISPI_INDEX_Y_OFFSET 0x9
#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40

static u32 framebuffer_address = 0;
static u16 framebuffer_width = 0;
static u16 framebuffer_height = 0;
static bool _is_enabled;

void BXVGA::initialise() {
	PCI::ID* BXVGA = PCI::find_id(0x1234, 0x1111);
    framebuffer_address = PCI::get_bar(BXVGA->addr, PCI_BAR_0) & 0xfffffff0;

	kdebugf("[BXVGA] framebuffer_address = %.8x\n", framebuffer_address);

	BXVGA::disable();
	set_resolution(1024, 768);
}

void BXVGA::set_register(u16 index, u16 data) {
	outw(VBE_DISPI_IOPORT_INDEX, index);
	outw(VBE_DISPI_IOPORT_DATA, data);
}

u16 BXVGA::get_register(u16 index) {
	 outw(VBE_DISPI_IOPORT_INDEX, index);
	 return inw(VBE_DISPI_IOPORT_DATA);
}

void BXVGA::set_resolution(u16 width, u16 height) {
	framebuffer_width = width;
	framebuffer_height = height;

    set_register(VBE_DISPI_INDEX_XRES, width);
    set_register(VBE_DISPI_INDEX_YRES, height);
    set_register(VBE_DISPI_INDEX_VIRT_WIDTH, width);
    set_register(VBE_DISPI_INDEX_VIRT_HEIGHT, height * 2);
    set_register(VBE_DISPI_INDEX_BPP, 32);
    set_register(VBE_DISPI_INDEX_BANK, 0);
}

void BXVGA::enable() {
	_is_enabled = true;
	set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

void BXVGA::disable() {
	_is_enabled = false;
	set_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
}

bool BXVGA::is_enabled() {
	return _is_enabled;
}

u32* BXVGA::framebuffer() {
	return (u32*) framebuffer_address;
}

u32 BXVGA::framebuffer_size() {
	return framebuffer_width * framebuffer_height * 2 * sizeof(u32);
}

u32 BXVGA::row_size() {
	return framebuffer_width * sizeof(u32);
}

u16 BXVGA::width() {
	return framebuffer_width;
}

u16 BXVGA::height() {
	return framebuffer_height;
}