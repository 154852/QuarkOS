#include <kernel/hardware/keyboard.hpp>
#include <stdio.h>
#include <string.h>
#include <kernel/hardware/pic.hpp>
#include <kernel/tty.hpp>
#include <assertions.h>
#include <kernel/kernel.hpp>
#include <kernel/socket.hpp>

#define SCAN_CODE_PORT 0x60
#define INTERRUPT_ID 0x01

static Keyboard::KeyboardState global_keyboard_state = { false, false, false };
#define CHAR_BUFFER_CAPACITY 64
static volatile char buffer[CHAR_BUFFER_CAPACITY];
static volatile u32 buffer_size = 0;

void Keyboard::push_to_buffer(char character) {
    assert(buffer_size < CHAR_BUFFER_CAPACITY);
    buffer[buffer_size] = character;
    buffer_size++;
}

void Keyboard::pop_from_buffer(u32 count) {
    assert((i32) buffer_size - (i32) count >= 0);
    buffer_size -= count;
}

u32 Keyboard::get_buffer_size() {
    return buffer_size;
}

char* Keyboard::get_buffer() {
    return (char*) buffer;
}

Socket::Socket* socket;
char write_buffer[sizeof(Keyboard::ScanCode) + sizeof(Keyboard::KeyboardState)];
void Keyboard::init() {
    socket = Socket::new_socket("/dev/keyboard");
}

__attribute__((interrupt)) void Keyboard::keyboard_interrupt(struct IRQ::CSITRegisters*) {
    PIC::send_EOI(INTERRUPT_ID);
    
    char rawcode = inb(SCAN_CODE_PORT);
    const ScanCode* code = scan_code(rawcode);

    if (code->name == KEY_LEFT_SHIFT || code->name == KEY_RIGHT_SHIFT) {
        global_keyboard_state.is_shifted = code->action == KEY_PRESS;
    }

    if (code->name == KEY_LEFT_CONTROL) {
        global_keyboard_state.is_ctrl = code->action == KEY_PRESS;
    }

    if (code->name == KEY_CAPS_LOCK) {
        global_keyboard_state.is_caps_locked = code->action == KEY_PRESS;
    }

    memcpy(write_buffer, code, sizeof(Keyboard::ScanCode));
    memcpy(write_buffer + sizeof(Keyboard::ScanCode), &global_keyboard_state, sizeof(Keyboard::KeyboardState));
    Socket::write_socket(socket, sizeof(Keyboard::ScanCode) + sizeof(Keyboard::KeyboardState), write_buffer);
}