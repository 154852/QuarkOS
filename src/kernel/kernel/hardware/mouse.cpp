#include <kernel/hardware/mouse.hpp>
#include <kernel/socket.hpp>
#include <kernel/hardware/pic.hpp>
#include <kernel/kernel.hpp>
#include <assertions.h>
#include <stdio.h>

#define I8042_BUFFER 0x60
#define I8042_STATUS 0x64
#define I8042_ACK 0xFA
#define I8042_RESEND 0xFE
#define I8042_BUFFER_FULL 0x01

#define I8042_WHICH_BUFFER 0x20

#define I8042_KEYBOARD_BUFFER 0x00
#define I8042_MOUSE_BUFFER 0x20

#define IRQ_MOUSE 12

#define PS2MOUSE_SET_RESOLUTION 0xE8
#define PS2MOUSE_STATUS_REQUEST 0xE9
#define PS2MOUSE_REQUEST_SINGLE_PACKET 0xEB
#define PS2MOUSE_GET_DEVICE_ID 0xF2
#define PS2MOUSE_SET_SAMPLE_RATE 0xF3
#define PS2MOUSE_ENABLE_PACKET_STREAMING 0xF4
#define PS2MOUSE_DISABLE_PACKET_STREAMING 0xF5
#define PS2MOUSE_SET_DEFAULTS 0xF6
#define PS2MOUSE_RESEND 0xFE
#define PS2MOUSE_RESET 0xFF

#define PS2MOUSE_INTELLIMOUSE_ID 0x03
#define PS2MOUSE_INTELLIMOUSE_EXPLORER_ID 0x04


void prepare_for_output() {
	while (inb(I8042_STATUS) & 2);
}

void prepare_for_input() {
	while (1) {
		u8 status = inb(I8042_STATUS);
		if (status & I8042_BUFFER_FULL)
			return;
	}
}

u8 read_from_device(u8 port) {
	prepare_for_input();
	return inb(port);
}

u8 read_from_device_buffer() {
	prepare_for_input();
	return inb(I8042_BUFFER);
}

void wait_then_write(u8 port, u8 data) {
	prepare_for_output();
	outb(port, data);
}

u8 write_to_device(u8 data) {
	prepare_for_output();
	outb(I8042_STATUS, 0xd4);
	prepare_for_output();
	outb(I8042_BUFFER, data);

	u8 res = read_from_device(I8042_BUFFER);
	assert(res != I8042_RESEND);

	return res;
}

u8 send_command(u8 command) {
	return write_to_device(command);
}

u8 send_command_with_data(u8 command, u8 data) {
	u8 response = write_to_device(command);
	if (response == I8042_ACK) response = write_to_device(data);
	return response;
}

bool reset_device() {
	if (send_command(0xff) != I8042_ACK) return false;
	return read_from_device(I8042_BUFFER) == 0xaa;
}

u8 read_device_id() {
	if (send_command(PS2MOUSE_GET_DEVICE_ID) != I8042_ACK) return 0;
	return read_from_device_buffer();
}

void drain_buffers() {
	while (1) {
		u8 status = inb(I8042_STATUS);
		if (!(status & I8042_BUFFER_FULL)) return;
		inb(I8042_BUFFER);
	}
}

static bool has_wheel = false;

bool Mouse::set_sample_rate(u8 sample_rate) {
	if (send_command_with_data(PS2MOUSE_SET_SAMPLE_RATE, sample_rate) != I8042_ACK) {
		return false;
	}

	return true;
}

static Socket::Socket* mouse_socket = Socket::new_socket("/dev/mouse");

void Mouse::init() {
	wait_then_write(I8042_STATUS, 0xad);
	wait_then_write(I8042_STATUS, 0xa7);

	drain_buffers();

	wait_then_write(I8042_STATUS, 0x20);
	u8 config = read_from_device(I8042_BUFFER);
	wait_then_write(I8042_STATUS, 0x60);
	config &= ~3;
	wait_then_write(I8042_BUFFER, config);

	assert((config & (1 << 5)) != 0); // Dual channel

	wait_then_write(I8042_STATUS, 0xaa);
	if (read_from_device(I8042_BUFFER) == 0x55) {
		wait_then_write(I8042_STATUS, 0x60);
		wait_then_write(I8042_BUFFER, config);
	} else {
		kdebugf("[Mouse] Failed self test\n");
		return;
	}

	wait_then_write(I8042_STATUS, 0xab);
	assert(read_from_device(I8042_BUFFER) == 0); // has keyboard
	wait_then_write(I8042_STATUS, 0xae);
	config |= 1;
	config &= ~(1 << 4);

	wait_then_write(I8042_STATUS, 0xa9);
	assert(read_from_device(I8042_BUFFER) == 0); // has mouse
	wait_then_write(I8042_STATUS, 0xa8);
	config |= 2;
	config &= ~(1 << 5);

	config &= ~0x30;
	wait_then_write(I8042_STATUS, 0x60);
	wait_then_write(I8042_BUFFER, config);

	reset_device();

	u8 device_id = read_from_device_buffer();
	
	if (send_command(PS2MOUSE_SET_DEFAULTS) != I8042_ACK) {
		kdebugf("[Mouse] Failed to set defaults\n");
		return;
	}

	if (send_command(PS2MOUSE_ENABLE_PACKET_STREAMING) != I8042_ACK) {
		kdebugf("[Mouse] Failed to enable packet streaming\n");
		return;
	}

	if (device_id != PS2MOUSE_INTELLIMOUSE_ID) {
		if (!set_sample_rate(200) || !set_sample_rate(100) || !set_sample_rate(80)) {
			kdebugf("[Mouse] Failed to setup intellimouse (sample rate)\n");
			return;
		}

		device_id = read_device_id();
	}

	if (device_id == PS2MOUSE_INTELLIMOUSE_ID) {
		has_wheel = true;
	}

	kdebugf("[Mouse] Initialised mouse, has_wheel=%d\n", has_wheel);
}

void __attribute__((interrupt)) Mouse::mouse_interrupt(struct IRQ::CSITRegisters*) {
	PIC::send_EOI(12);

	if (!(inb(I8042_STATUS) & I8042_BUFFER_FULL)) return;

	MousePacket packet = {
		.flags = MousePacketFlags { .raw = inb(I8042_BUFFER) },
		.x_delta = inb(I8042_BUFFER),
		.y_delta = inb(I8042_BUFFER),
		.scroll_delta_x = 0,
		.scroll_delta_y = 0
	};

	unsigned scroll_delta = inb(I8042_BUFFER);

	if (packet.flags.x_overflow || packet.flags.y_overflow) return;

	if (packet.flags.x_sign) packet.x_delta |= 0xFFFFFF00;
	if (packet.flags.y_sign) packet.y_delta |= 0xFFFFFF00;

	packet.scroll_delta_y = (char) scroll_delta;

	Socket::write_socket(mouse_socket, sizeof(MousePacket), &packet);
}