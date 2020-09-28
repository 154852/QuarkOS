#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H
 
#include <stdint2.h>

namespace Terminal {
	void initialize();
	void putchar(char c);
	void write(const char* data, u32 size);
	void writestring(const char* data);
	void char_back();
	void set_color(u8 color);
	void write(const char* data, u32 size);
	void put_entry_at(unsigned char c, u8 color, u32 x, u32 y);
	void scroll_up(u32 count);
	void reset();
};
 
#endif