#ifndef _KERNEL_SOCKET_H
#define _KERNEL_SOCKET_H

#include <stdint2.h>
#include <stddef.h>

namespace Socket {
	struct Socket {
		bool present;
		
		unsigned id;
		void* data;
		unsigned length;

		void* generation_id;
		unsigned(*generate)(void* id, void* data, unsigned length);
	};

	Socket* new_socket_with_gen(void* gen_id, unsigned(*generate)(void* id, void* data, unsigned length));
	Socket* new_socket();
	Socket* socket_from_id(unsigned id);
	unsigned read_socket(Socket* socket, unsigned length, void* data);
	void write_socket(Socket* socket, unsigned length, void* data);
	Socket* all(size_t* count);
}

#endif