#include <kernel/hardware/keyboard.hpp>
#include <stdio.h>
#include <kernel/hardware/pic.hpp>
#include <kernel/tty.hpp>
#include <assertions.h>

#define SCAN_CODE_PORT 0x60
#define INTERRUPT_ID 0x01

static Keyboard::KeyboardState global_keyboard_state = { false, false };
#define CHAR_BUFFER_CAPACITY 64
static char buffer[CHAR_BUFFER_CAPACITY];
static u32 buffer_size = 0;

void Keyboard::push_to_buffer(char character) {
    assert(buffer_size < CHAR_BUFFER_CAPACITY);
    buffer[buffer_size] = character;
    buffer_size++;
}

void Keyboard::pop_from_buffer(u32 count) {
    assert(buffer_size - count >= 0);
    buffer_size -= count;
}

u32 Keyboard::get_buffer_size() {
    return buffer_size;
}

char* Keyboard::get_buffer() {
    return buffer;
}

char Keyboard::scan_code_to_char(const Keyboard::ScanCode* code, Keyboard::KeyboardState* state) {
    if (!code) return 0;
    if (code->name == KEY_UNKNOWN) return 0;

    if (!state->is_shifted && code->action == KEY_PRESS && code->name >= KEY_0 && code->name <= KEY_9)
        return (code->name - KEY_0) + '0';
    else if (code->action == KEY_PRESS && code->name >= KEY_A && code->name <= KEY_Z)
        return (code->name - KEY_A) + ((state->is_caps_locked != state->is_shifted)? 'A':'a');
    else if (code->action == KEY_PRESS && code->name == KEY_SPACE) return ' ';
    else if (code->action == KEY_PRESS && code->name == KEY_ENTER) return '\n';
    else if (code->action == KEY_PRESS && code->name == KEY_TAB) return '\t';
    
    if (state->is_shifted && code->action == KEY_PRESS) {
        switch (code->name) {
            case KEY_1: return '!';
            case KEY_2: return '@';
            case KEY_3: return 0;
            case KEY_4: return '$';
            case KEY_5: return '%';
            case KEY_6: return '^';
            case KEY_7: return '&';
            case KEY_8: return '*';
            case KEY_9: return '(';
            case KEY_0: return ')';
            case KEY_LEFT_BRACKET: return '{';
            case KEY_RIGHT_BRACKET: return '}';
            case KEY_DASH: return '_';
            case KEY_EQUALS: return '+';
            case KEY_SEMI_COLON: return ':';
            case KEY_SINGLE_QUOTE: return '"';
            case KEY_BACK_SLASH: return '|';
            case KEY_BACK_TICK: return '~';
            case KEY_COMMA: return '<';
            case KEY_DOT: return '>';
            case KEY_SLASH: return '?';
            default: {}
        }
    } else if (code->action == KEY_PRESS) {
        switch (code->name) {
            case KEY_DASH: return '-';
            case KEY_EQUALS: return '=';
            case KEY_LEFT_BRACKET: return '[';
            case KEY_RIGHT_BRACKET: return ']';
            case KEY_SEMI_COLON: return ';';
            case KEY_SINGLE_QUOTE: return '\'';
            case KEY_BACK_SLASH: return '\\';
            case KEY_BACK_TICK: return '`';
            case KEY_COMMA: return ',';
            case KEY_DOT: return '.';
            case KEY_SLASH: return '/';
            default: {}
        }
    }

    return 0;
}

__attribute__((interrupt)) void Keyboard::keyboard_interrupt(struct IRQ::CSITRegisters*) {
    PIC::send_EOI(INTERRUPT_ID);
    
    const ScanCode* code = scan_code(inb(SCAN_CODE_PORT));

    if (code->name == KEY_LEFT_SHIFT || code->name == KEY_RIGHT_SHIFT) {
        global_keyboard_state.is_shifted = code->action == KEY_PRESS;
        return;
    }

    if (code->name == KEY_CAPS_LOCK) {
        global_keyboard_state.is_caps_locked = code->action == KEY_PRESS;
        return;
    }

    if (code->name == KEY_BACKSPACE && code->action == KEY_PRESS) {
        Terminal::char_back();
        return;
    }

    char ch = scan_code_to_char(code, &global_keyboard_state);
    // if (ch) printf("%c", ch);
    if (ch) push_to_buffer(ch);
}