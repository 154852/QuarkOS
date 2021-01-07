#include <string.h>
#include <stdio.h>

#include <kernel/tty.hpp>
#include <kernel/vga.hpp>

static const u32 VGA_WIDTH = 80;
static const u32 VGA_HEIGHT = 25;
static u16* const VGA_MEMORY = (u16*) 0xB8000;

static u32 terminal_row;
static u32 terminal_column;
static u8 terminal_color;
static u16* terminal_buffer;

void Terminal::initialize(void) {
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = VGA_MEMORY;
	for (u32 y = 0; y < VGA_HEIGHT; y++) {
		for (u32 x = 0; x < VGA_WIDTH; x++) {
			const u32 index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void Terminal::set_color(u8 color) {
	terminal_color = color;
}

void Terminal::put_entry_at(unsigned char c, u8 color, u32 x, u32 y) {
	const u32 index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void Terminal::putchar(char c) {
	if (c == '\n') {
		terminal_row++;
		if (terminal_row >= VGA_HEIGHT) Terminal::scroll_up(1);
		terminal_column = 0;
		return;
	}

	unsigned char uc = c;
	Terminal::put_entry_at(uc, terminal_color, terminal_column, terminal_row);
	if (++terminal_column == VGA_WIDTH) {
		terminal_column = 0;
		if (++terminal_row == VGA_HEIGHT) Terminal::scroll_up(1);
	}
}

void Terminal::write(const char* data, u32 size) {
	for (u32 i = 0; i < size; i++)
		Terminal::putchar(data[i]);
}

void Terminal::writestring(const char* data) {
	Terminal::write(data, strlen(data));
}

void Terminal::scroll_up(u32 count) {
	u32 size = count * VGA_WIDTH * sizeof(u16);
	u32 total_size = VGA_HEIGHT * VGA_WIDTH * sizeof(u16);
	memmove((u8*) terminal_buffer, (u8*) terminal_buffer + size, total_size - size);
	memset((u8*) terminal_buffer + total_size - size, 0, size);
	terminal_row--;
}

void Terminal::char_back() {
	if (terminal_row == 0 && terminal_column == 0) return;

	if (terminal_column != 0) {
		terminal_column--;
		Terminal::put_entry_at(' ', terminal_color, terminal_column, terminal_row);
	} else terminal_row--;
}

void Terminal::reset() {
	Terminal::initialize();
}