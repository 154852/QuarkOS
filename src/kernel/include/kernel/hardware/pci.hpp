#include <stdint2.h>

#ifndef _KERNEL_PCI_H
#define _KERNEL_PCI_H

#define PCI_BAR_0 0x10
#define PCI_BAR_1 0x14
#define PCI_BAR_2 0x18
#define PCI_BAR_3 0x1C
#define PCI_BAR_4 0x20
#define PCI_BAR_5 0x24

namespace PCI {
	union Address {
		struct {
			u8 reg : 8;
			u8 function : 3;
			u8 device : 5;
			u8 bus : 8;
			u8 reserved : 7;
			u8 enable : 1;
		};

		u32 raw;
	};

	struct ID {
		bool present = false;

		Address addr;
		u16 vendor_id;
		u16 device_id;
	};

	u8 read_reg8(Address addr, u8 off);
	u16 read_reg16(Address addr, u8 off);
	u32 read_reg32(Address addr, u8 off);

	void write_reg8(Address addr, u8 off, u8 val);
	void write_reg16(Address addr, u8 off, u16 val);
	void write_reg32(Address addr, u8 off, u32 val);

	u16 read_type(Address addr);
	
	void load_hardware();
	ID* get_ids();
	ID* find_id(u16 vendor_id, u16 device_id);

	u32 get_bar(Address addr, u32 bar);
}

#endif