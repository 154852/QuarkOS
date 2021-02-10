#include "ext2/init.hpp"
#include <ext2/path.hpp>
#include <string.h>
#include <stdarg.h>

ext2::INode* ext2::inode_from_path(const char* path, ext2::INode* start) {
	unsigned inode = inode_id_from_path(path, start);
	if (inode == 0) return 0;
	return ext2::get_tmp_inode_at(inode);
}

ext2::INode* ext2::inode_from_relative_path(const char* path, INode* start) {
	if (path[0] == '/') return inode_from_path(path, get_tmp_root_inode());
	return inode_from_path(path, start);
}

ext2::INode* ext2::inode_from_root_path(const char* path) {
	return inode_from_path(path, get_tmp_root_inode());
}

unsigned ext2::inode_id_from_path(const char* path, ext2::INode* start) {
	while (path[0] == '/') path++;

	unsigned id = 0;
	ext2::INode* node = start;

	int len = strlen(path);
	int last_component = 0;
	for (int i = 1; i < len; i++) {
		if (path[i] == '/') {
			ext2::DirectoryEntry* entry = ext2::find_tmp_direntryl(node, path + last_component, i - last_component);
			if (entry == 0) return 0;

			id = entry->inode;
			node = ext2::get_tmp_inode_at(entry->inode);
			if (!ext2::is_dir(node)) return 0;
			if (node == 0) return 0;

			while (path[i] == '/') i++;
			last_component = i;
		}
	}

	if (last_component == len) return id;

	ext2::DirectoryEntry* entry = ext2::find_tmp_direntryl(node, path + last_component, len - last_component);
	if (entry == 0) return 0;

	return entry->inode;
}

unsigned ext2::inode_id_from_relative_path(const char* path, INode* start) {
	if (path[0] == '/') return inode_id_from_path(path, get_tmp_root_inode());
	return inode_id_from_path(path, start);
}

unsigned ext2::inode_id_from_root_path(const char* path) {
	return inode_id_from_path(path, get_tmp_root_inode());
}

char* ext2::path_join(int count, ...) {
	va_list ap;
	va_start(ap, count);

	static char path[256];
	size_t idx = 0;

	memset(path, 0, sizeof(path));
	for (int i = 1; i <= count; i++) {
		char* name = va_arg(ap, char*);
		size_t len = strlen(name);
		memcpy(path + idx, name, len);
		idx += len;

		if (i != count) path[idx++] = '/';
	}

	va_end(ap);
	return path;
}