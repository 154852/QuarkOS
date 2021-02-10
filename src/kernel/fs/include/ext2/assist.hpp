#include <ext2/path.hpp>

namespace ext2 {
	bool new_socket(unsigned node, const char* name, unsigned id);
	bool mkdir(unsigned node, const char* name);
	bool new_text_file(unsigned node, const char* name, const char* string);
};