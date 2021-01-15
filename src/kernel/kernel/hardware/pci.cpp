#include <assertions.h>
#include <kernel/kernel.hpp>
#include <kernel/hardware/pci.hpp>
#include <stdio.h>

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC
#define PCI_HEADER_TYPE 0x0e
#define PCI_VENDOR_ID 0x00
#define PCI_NONE 0xFFFF
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_TYPE_BRIDGE 0x0604
#define PCI_SECONDARY_BUS 0x19
#define PCI_DEVICE_ID 0x02

#define DEVICES_CAPACITY 64
static PCI::ID devices[DEVICES_CAPACITY];
static u32 loaded_devices = 0;

u32 PCI::read_reg32(PCI::Address addr, u8 reg) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	return indw(PCI_VALUE_PORT);
}

u16 PCI::read_reg16(PCI::Address addr, u8 reg) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	return inw(PCI_VALUE_PORT + (reg & 2));
}

u8 PCI::read_reg8(PCI::Address addr, u8 reg) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	return inb(PCI_VALUE_PORT + (reg & 3));
}


void PCI::write_reg32(PCI::Address addr, u8 reg, u32 value) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	outdw(PCI_VALUE_PORT, value);
}

void PCI::write_reg16(PCI::Address addr, u8 reg, u16 value) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	outw(PCI_VALUE_PORT + (reg & 2), value);
}

void PCI::write_reg8(PCI::Address addr, u8 reg, u8 value) {
	PCI::Address address;
	address.raw = addr.raw;
	address.enable = 1;
	address.reg = reg & 0xfc;

	outdw(PCI_ADDRESS_PORT, address.raw);
	outb(PCI_VALUE_PORT + (reg & 3), value);
}

u16 PCI::read_type(PCI::Address addr) {
	return (PCI::read_reg8(addr, PCI_CLASS) << 8) | PCI::read_reg8(addr, PCI_SUBCLASS);
}

void load_bus(int type, u8 bus);

void load_functions(int type, u8 bus, u8 device, u8 function) {
	PCI::Address address;
	address.raw = 0;
	address.bus = bus;
	address.device = device;
	address.function = function;

	if (type == -1 || type == PCI::read_type(address)) {
		assert(loaded_devices < DEVICES_CAPACITY);
		devices[loaded_devices].present = true;
		devices[loaded_devices].addr = address;
		devices[loaded_devices].vendor_id = PCI::read_reg16(address, PCI_VENDOR_ID);
		devices[loaded_devices].device_id = PCI::read_reg16(address, PCI_DEVICE_ID);

		loaded_devices++;
	}

	if (PCI::read_type(address) == PCI_TYPE_BRIDGE) {
		u8 secondary_bus = PCI::read_reg8(address, PCI_SECONDARY_BUS);
		assert(secondary_bus != bus);
		load_bus(type, secondary_bus);
	}
}

void load_device(int type, u8 bus, u8 device) {
	PCI::Address address;
	address.raw = 0;
	address.bus = bus;
	address.device = device;

	if (PCI::read_reg16(address, PCI_VENDOR_ID) == PCI_NONE) return;
	load_functions(type, bus, device, 0);
	if (!(PCI::read_reg8(address, PCI_HEADER_TYPE) & 0x80)) return;
	for (u8 function = 1; function < 8; function++) {
		address.function = function;
		if (PCI::read_reg16(address, PCI_VENDOR_ID) != PCI_NONE) load_functions(type, bus, device, function);
	}
}

void load_bus(int type, u8 bus) {
	for (u8 device = 0; device < 32; device++) {
		load_device(type, bus, device);
	}
}

void PCI::load_hardware() {
	if ((read_reg8(Address(), PCI_HEADER_TYPE) & 0x80) == 0) {
		load_bus(-1, 0);
		return;
	}

	assert(false);
}

PCI::ID* PCI::get_ids() {
	return devices;
}

PCI::ID* PCI::find_id(u16 vendor_id, u16 device_id) {
	for (u32 i = 0; i < loaded_devices; i++) {
		if (devices[i].vendor_id == vendor_id && devices[i].device_id == device_id) return &devices[i];
	}
	return 0;
}

u32 PCI::get_bar(PCI::Address addr, u32 bar) {
	return PCI::read_reg32(addr, bar);
}