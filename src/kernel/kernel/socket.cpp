#include <kernel/socket.hpp>
#include <kernel/kmalloc.hpp>
#include <assertions.h>
#include <string.h>

#define SOCKETS_CAPACITY 100
static Socket::Socket sockets[SOCKETS_CAPACITY];

Socket::Socket* Socket::socket_from_id(unsigned id) {
	assert(id < SOCKETS_CAPACITY);
	return &sockets[id];
}

Socket::Socket* Socket::new_socket_with_gen(void* gen_id, unsigned(*generate)(void* id, void* data, unsigned length)) {
	Socket* sock = new_socket();
	sock->generate = generate;
	sock->generation_id = gen_id;
	return sock;
}

Socket::Socket* Socket::new_socket() {
	for (int i = 0; i < SOCKETS_CAPACITY; i++) {
		if (!sockets[i].present) {
			sockets[i].present = true;
			sockets[i].length = 0;
			sockets[i].data = 0;
			sockets[i].id = i;
			return &sockets[i];
		}
	}
	return 0;
}

unsigned Socket::read_socket(Socket* socket, unsigned length, void* data) {
	if (socket->generate != 0) {
		return socket->generate(socket->generation_id, data, length);
	}

	if (socket->length < length) {
		memcpy(data, socket->data, socket->length);
		unsigned length = socket->length;
		socket->data = 0; // free?
		socket->length = 0;
		return length;
	}

	memcpy(data, socket->data, length); // read from the start
	memmove(socket->data, (const void*) ((unsigned) socket->data + length), socket->length - length);
	socket->length -= length;

	return length;
}

void Socket::write_socket(Socket* socket, unsigned length, void* data) {
	if (socket->generate != 0) return;

	void* raw = kmalloc(socket->length + length);
	if (socket->length != 0) memcpy(raw, socket->data, socket->length);
	memcpy((void*) ((unsigned) raw + socket->length), data, length); // write to the end
	// free socket->data
	socket->length += length;
	socket->data = raw;
}

Socket::Socket* Socket::all(size_t* count) {
	*count = SOCKETS_CAPACITY;
	return sockets;
}