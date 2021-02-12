#include <kernel/kmalloc.hpp>
#include <assertions.h>
#include <kernel/kernel.hpp>
#include <ext2/init.hpp>
#include <kernel/hardware/disk.hpp>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define read_u32(from, idx) (*(unsigned*) &from[idx])
#define read_u16(from, idx) (*(unsigned short*) &from[idx])

#ifdef __is_local
	#define log(...) printf(__VA_ARGS__)
#else
	#define log(...) kdebugf(__VA_ARGS__)
#endif

// #define __debug_root

#ifdef __debug_root
void find(ext2::INode dir, const char* name) {
	int count = dir_entry_count(&dir);
	for (int i = 0; i < count; i++) {
		ext2::DirectoryEntry* entry = read_tmp_direntry(&dir, i);
		if (strcmp(entry->name, ".") == 0 || strcmp(entry->name, "..") == 0) continue;

		ext2::INode* inode = ext2::get_tmp_inode_at(entry->inode);
		debugf("%s%s%s, size=%d\n", name, entry->name, ext2::is_dir(inode)? "/":"", inode->size_low);
		if (ext2::is_dir(inode)) {
			char name_build[256]; memset(name_build, 0, 256);
			int len = 0;

			int l2 = strlen(name); memcpy(name_build + len, name, l2);
			len += l2;

			l2 = strlen(entry->name); memcpy(name_build + len, entry->name, l2);
			len += l2;

			name_build[len] = '/';

			find(*inode, name_build);
		}
	}
}
#endif

char superblock_raw[1024];
ext2::SuperBlock* superblock = (ext2::SuperBlock*) superblock_raw;
size_t block_group_count;

void ext2::init() {
	Disk::read_sectors(2, 2, (u8*) superblock);

	superblock->block_size = 1024 << superblock->block_size;
	superblock->fragment_size = 1024 << superblock->fragment_size;

	assert(superblock->signature == 0xef53);
	assert(superblock->state == FS_CLEAN);

	block_group_count = superblock->block_count % superblock->blocks_per_block_group == 0? (superblock->block_count / superblock->blocks_per_block_group) + 1:(superblock->block_count / superblock->blocks_per_block_group);
	log("[ext2fs] Block size=%d, block groups: %d, blocks: %d, inodes: %d, blocks/block group: %d, inodes/block group: %d\n", superblock->block_size, block_group_count, superblock->block_count, superblock->inode_count, superblock->blocks_per_block_group, superblock->inodes_per_block_group);


	assert(superblock->block_size == 1024);

#ifdef __debug_root
	ext2::DirectoryEntry dirent;
	char raw[512];

	unsigned newdir = allocate_inode();
	assert(newdir);
	ext2::init_dir_empty(newdir, 2);

	unsigned newfile = allocate_inode();
	assert(newfile);
	ext2::init_file_empty(newfile);
	memset(raw, 0, sizeof(raw));
	memcpy(raw, (char*) "Hello World!", 13);
	ext2::write_file_content(newfile, get_tmp_inode_at(newfile), 0, raw, sizeof(raw));

	dirent.inode = newfile;
	dirent.name = (char*) "test2.txt";
	dirent.name_len_low = 9;
	dirent.type_or_name_len_high = DIRENT_TYPE_FILE;
	ext2::insert_dirent(&dirent, get_tmp_inode_at(newdir));

	dirent.inode = newdir;
	dirent.name = (char*) "test";
	dirent.name_len_low = 4;
	dirent.type_or_name_len_high = DIRENT_TYPE_DIR;
	ext2::INode* root = get_tmp_root_inode();
	ext2::insert_dirent(&dirent, root);
	find(*root, "/");

	log("Find end \n");

	char out[100];
	ext2::read_file_content(get_tmp_inode_at(newfile), 0, out, 13);
	debugf("Out: %s\n", out);
#endif

	log("[ext2fs] Initialised file system\n");
}

unsigned ext2::block_group_for(unsigned inode) {
	return (inode - 1) / superblock->inodes_per_block_group;
}

unsigned ext2::inode_in_group_for(unsigned inode) {
	return (inode - 1) % superblock->inodes_per_block_group;
}

void ext2::read_raw(unsigned addr, void* out, unsigned length) {
	unsigned sectors = (addr % 512) + length;
	sectors = sectors % 512 == 0? sectors / 512:((sectors / 512) + 1);
	Disk::read_sectors_limited(addr / 512, sectors, (u8*) out, addr % 512, length);
}

void ext2::write_raw(unsigned addr, void* data, unsigned length) {
	assert(addr % 512 == 0);
	assert(length % 512 == 0);

	// TODO: Allow writing not in whole sectors
	unsigned sectors = (addr % 512) + length;
	sectors = sectors % 512 == 0? sectors / 512:((sectors / 512) + 1);
	Disk::write_sectors(addr / 512, sectors, (u8*) data);
}

ext2::BlockGroupDescriptor* ext2::block_group_descriptor_at(unsigned index) {
	static ext2::BlockGroupDescriptor descriptor;
	ext2::read_raw((index * 32) + ((superblock->superblock_block_idx + 1) * superblock->block_size), &descriptor, sizeof(descriptor));
	return &descriptor;
}

ext2::INode* ext2::get_tmp_inode_at(unsigned index) {
	if (index == 0) return 0;
	
	BlockGroupDescriptor* block_group = block_group_descriptor_at(block_group_for(index));
	size_t inode_table = block_group->inode_table_addr * superblock->block_size;
	static ext2::INode inode;
	ext2::read_raw(inode_table + (inode_in_group_for(index) * sizeof(INode)), &inode, sizeof(INode));
	return &inode;
}

static char block[1024];
void ext2::write_inode_at(unsigned index, ext2::INode* node) {
	BlockGroupDescriptor* block_group = block_group_descriptor_at(block_group_for(index));
	size_t inode_table = block_group->inode_table_addr * superblock->block_size;
	size_t addr = inode_table + (((ext2::inode_in_group_for(index) * 128) / superblock->block_size) * superblock->block_size);
	size_t in_block_addr = (inode_in_group_for(index) * sizeof(INode)) % superblock->block_size;
	ext2::read_raw(addr, block, superblock->block_size);
	memcpy(block + in_block_addr, node, sizeof(ext2::INode));
	ext2::write_raw(addr, block, superblock->block_size);
}

ext2::INode* ext2::get_tmp_root_inode() {
	return get_tmp_inode_at(2);
}

#define min(a, b) ((a) < (b)? (a):(b))
#define max(a, b) ((a) > (b)? (a):(b))

unsigned singly_indirect_block[256];
unsigned doubly_indirect_block[256];
void ext2::read_file_content(ext2::INode* node, unsigned off, void* out, unsigned length) {
	unsigned done = 0;

	// Direct block pointers
	for (int i = 0; i < 12; i++) {
		if (off >= superblock->block_size) {
			off -= superblock->block_size;
			continue;
		}

		if (done < length) {
			ext2::read_raw((node->direct_block_pointers[i] * superblock->block_size) + off, (char*) out + done, min(superblock->block_size - off, length - done));
			done += superblock->block_size;
			off = 0;
		} else {
			break;
		}
	}

	if (done >= length) return;
	
	// Singly indirect block pointers
	ext2::read_raw(node->singly_indirect_block_pointer * superblock->block_size, singly_indirect_block, superblock->block_size);
	for (int i = 0; i < 256; i++) {
		if (off >= superblock->block_size) {
			off -= superblock->block_size;
			continue;
		}

		if (done < length) {
			ext2::read_raw((singly_indirect_block[i] * superblock->block_size) + off, (char*) out + done, min(superblock->block_size - off, length - done));
			done += superblock->block_size;
			off = 0;
		} else {
			break;
		}
	}

	if (done >= length) return;

	// Doubly indirect block pointers
	ext2::read_raw(node->doubly_indirect_block_pointer * superblock->block_size, doubly_indirect_block, superblock->block_size);
	for (int j = 0; j < 256; j++) {
		if (off >= superblock->block_size * 256) {
			off -= superblock->block_size;
			continue;
		}

		ext2::read_raw(doubly_indirect_block[j] * superblock->block_size, singly_indirect_block, superblock->block_size);
		for (int i = 0; i < 256; i++) {
			if (off >= superblock->block_size) {
				off -= superblock->block_size;
				continue;
			}

			if (done < length) {
				ext2::read_raw((singly_indirect_block[i] * superblock->block_size) + off, (char*) out + done, min(superblock->block_size - off, length - done));
				done += superblock->block_size;
				off = 0;
			} else {
				break;
			}
		}
	}

	if (done >= length) return;

	// TODO: Add indirect block pointers
	assert(done >= length);
}

void ext2::write_file_content(unsigned nodeIdx, ext2::INode* node, unsigned off, void* data, unsigned length) {
	unsigned done = 0;

	node->size_low = max(off + length, node->size_low);
	ext2::write_inode_at(nodeIdx, node);

	// Direct block pointers
	for (int i = 0; i < 12; i++) {
		if (off >= superblock->block_size) {
			off -= superblock->block_size;
			continue;
		}

		if (done < length) {
			if (node->direct_block_pointers[i] == 0) {
				node->direct_block_pointers[i] = ext2::allocate_block();
				assert(node->direct_block_pointers[i]);
				ext2::write_inode_at(nodeIdx, node);
			}
			ext2::write_raw((node->direct_block_pointers[i] * superblock->block_size) + off, (char*) data + done, min(superblock->block_size - off, length - done));
			done += superblock->block_size;
			off = 0;
		} else {
			break;
		}
	}

	if (done >= length) return;
	
	// TODO: Add indirect block pointers
	assert(done >= length);
}

ext2::DirectoryEntry* ext2::read_tmp_direntry(ext2::INode* node, unsigned idx) {
	assert(ext2::is_dir(node));

	static ext2::DirectoryEntry entry;
	static char name[256];

	unsigned i = 0;

	for (unsigned count = 0; count <= idx && i < superblock->block_size;) {
		ext2::read_file_content(node, i, &entry, sizeof(ext2::DirectoryEntry) - 4);

		if (count == idx) {
			memset(name, 0, sizeof(name));
			ext2::read_file_content(node, i + sizeof(ext2::DirectoryEntry) - 4, name, entry.name_len_low);
			entry.name = name;

			return &entry;
		}
	
		if (entry.inode != 0) count++;
		i += entry.size;
	}

	return 0;
}

unsigned ext2::dir_entry_count(INode *node) {
	assert(ext2::is_dir(node));

	static ext2::DirectoryEntry entry;

	unsigned i = 0;
	unsigned count = 0;
	while (i < superblock->block_size) {
		ext2::read_file_content(node, i, &entry, sizeof(ext2::DirectoryEntry) - 4);
		i += entry.size;
		if (entry.inode == 0) continue;
		count++;
	}
	return count;
}

bool ext2::is_dir(INode* node) {
	return ((node->type_and_permissions & 0x0000f000) == TYPE_DIRECTORY) != 0;
}

ext2::DirectoryEntry* ext2::find_tmp_direntry(INode* node, const char* name) {
	assert(ext2::is_dir(node));

	static ext2::DirectoryEntry entry;
	static char name_cpy[256];

	unsigned i = 0;
	while (i < superblock->block_size) {
		ext2::read_file_content(node, i, &entry, sizeof(ext2::DirectoryEntry) - 4);
		if (entry.inode != 0) {
			memset(name_cpy, 0, sizeof(name_cpy));
			ext2::read_file_content(node, i + sizeof(ext2::DirectoryEntry) - 4, name_cpy, entry.name_len_low);
			if (strcmp(name_cpy, name) == 0) {
				entry.name = name_cpy;
				return &entry;
			}
		}

		i += entry.size;
	}

	return 0;
}

ext2::DirectoryEntry* ext2::find_tmp_direntryl(INode* node, const char* name, unsigned len) {
	assert(ext2::is_dir(node));

	static ext2::DirectoryEntry entry;
	static char name_cpy[256];

	unsigned i = 0;
	while (i < superblock->block_size) {
		ext2::read_file_content(node, i, &entry, sizeof(ext2::DirectoryEntry) - 4);
		if (entry.inode != 0) {
			memset(name_cpy, 0, sizeof(name_cpy));
			ext2::read_file_content(node, i + sizeof(ext2::DirectoryEntry) - 4, name_cpy, entry.name_len_low);
			if (strlen(name_cpy) == len && memcmp(name_cpy, name, len) == 0) {
				entry.name = name_cpy;
				return &entry;
			}
		}

		i += entry.size;
	}

	return 0;
}

static char usage_bitmap[1024];
unsigned ext2::allocate_block() {
	for (unsigned i = 0; i < block_group_count; i++) {
		ext2::BlockGroupDescriptor* group  = ext2::block_group_descriptor_at(i);
		if (group->unallocated_blocks_in_group == 0) continue;

		ext2::read_raw(group->block_usage_bitmap_addr * superblock->block_size, usage_bitmap, sizeof(usage_bitmap));
		for (unsigned j = 0; j < superblock->blocks_per_block_group && j < sizeof(usage_bitmap) * 8; j++) {
			if ((usage_bitmap[j / 8] & (1 << (j % 8))) == 0) {
				usage_bitmap[j / 8] |= 1 << (j % 8);
				ext2::write_raw(group->block_usage_bitmap_addr * superblock->block_size, usage_bitmap, sizeof(usage_bitmap));
				return group->inode_table_addr + ((superblock->inodes_per_block_group * 128) / superblock->block_size) + j;
			}
		}
	}

	kdebugf("[ext2] Run out of unallocated blocks, allocate more in buildfs.py\n");

	return 0;
}

unsigned ext2::allocate_inode() {
	unsigned failed = 0;
	for (unsigned i = 0; i < block_group_count; i++) {
		ext2::BlockGroupDescriptor* group  = ext2::block_group_descriptor_at(i);
		if (group->unallocated_nodes_in_group == 0) continue;
		failed += superblock->inodes_per_block_group;

		ext2::read_raw(group->inode_usage_bitmap_addr * superblock->block_size, usage_bitmap, sizeof(usage_bitmap));
		for (unsigned j = 0; j < superblock->inodes_per_block_group && j < sizeof(usage_bitmap) * 8; j++) {
			if ((usage_bitmap[j / 8] & (1 << (j % 8))) == 0) {
				usage_bitmap[j / 8] |= 1 << (j % 8);
				ext2::write_raw(group->inode_usage_bitmap_addr * superblock->block_size, usage_bitmap, sizeof(usage_bitmap));
				return (i * superblock->inodes_per_block_group) + j + 1;
			}
			failed++;
		}
	}


	kdebugf("[ext2] Run out of unallocated inodes, allocate more in buildfs.py\n");
	return 0;
}

static char dirents[1024];
bool ext2::insert_dirent(ext2::DirectoryEntry* entry, ext2::INode* dir) {
	assert(ext2::is_dir(dir));

	ext2::DirectoryEntry* oldentry = 0;
	ext2::read_file_content(dir, 0, dirents, superblock->block_size);

	unsigned i = 0;
	unsigned count = 0;
	unsigned lastI = 0;
	while (i < superblock->block_size) {
		ext2::DirectoryEntry* nextentry = (ext2::DirectoryEntry*) (dirents + i);
		if (nextentry->inode != 0) oldentry = nextentry;
		lastI = i;
		i += nextentry->size;
		count++;
	}

	unsigned need_size = sizeof(ext2::DirectoryEntry) - 4 + entry->name_len_low;

	if (oldentry == 0) {
		assert(lastI == 0);

		if (superblock->block_size < need_size) return false;
		entry->size = superblock->block_size;

		memset(dirents, 0, sizeof(dirents));
		memcpy(dirents, entry, sizeof(ext2::DirectoryEntry) - 4);
		memcpy(dirents + sizeof(ext2::DirectoryEntry) - 4, entry->name, entry->name_len_low);

		ext2::write_raw(dir->direct_block_pointers[0] * superblock->block_size, dirents, superblock->block_size);
		return true;
	}

	unsigned oldloc = i - oldentry->size;

	unsigned real_size = sizeof(ext2::DirectoryEntry) - 4 + oldentry->name_len_low;
	real_size += real_size % 4 == 0? 0:(4 - (real_size % 4)); // 4 byte align

	unsigned available = superblock->block_size - (oldloc + real_size);
	if (available < need_size) return false;

	oldentry->size = real_size;
	entry->size = available;

	memset(dirents + oldloc + real_size + sizeof(ext2::DirectoryEntry) - 4 + entry->name_len_low, 0, available - (sizeof(ext2::DirectoryEntry) - 4 + entry->name_len_low));
	memcpy(dirents + oldloc + real_size, entry, sizeof(ext2::DirectoryEntry) - 4);
	memcpy(dirents + oldloc + real_size + sizeof(ext2::DirectoryEntry) - 4, entry->name, entry->name_len_low);

	ext2::write_raw(dir->direct_block_pointers[0] * superblock->block_size, dirents, superblock->block_size);
	return true;
}

void ext2::init_dir_empty(unsigned inode, unsigned parent) {
	ext2::INode* dir = get_tmp_inode_at(inode);
	memset(dir, 0, sizeof(ext2::INode));

	dir->direct_block_pointers[0] = allocate_block();
	dir->type_and_permissions |= TYPE_DIRECTORY;

	dir->size_low = 1024;

	memset(dirents, 0, sizeof(dirents));
	ext2::DirectoryEntry* ent = (ext2::DirectoryEntry*) dirents;
	ent->inode = inode;
	ent->name_len_low = 1;
	ent->type_or_name_len_high = DIRENT_TYPE_DIR;
	ent->size = sizeof(ext2::DirectoryEntry) - 4 + 1;
	*((char*) &ent->name) = '.';
	
	ent = (ext2::DirectoryEntry*) (dirents + ent->size);
	ent->inode = parent;
	ent->name_len_low = 2;
	ent->type_or_name_len_high = DIRENT_TYPE_DIR;
	ent->size = superblock->block_size - (sizeof(ext2::DirectoryEntry) - 4 + 1);
	*((char*) &ent->name) = '.';
	*((char*) &ent->name + 1) = '.';

	ext2::write_raw(dir->direct_block_pointers[0] * superblock->block_size, dirents, superblock->block_size);
	ext2::write_inode_at(inode, dir);
}

void ext2::init_file_empty(unsigned inode) {
	ext2::INode* file = get_tmp_inode_at(inode);
	memset(file, 0, sizeof(ext2::INode));

	file->type_and_permissions |= TYPE_FILE;
	file->size_low = 0;

	ext2::write_inode_at(inode, file);
}

void ext2::init_socket(unsigned inode, unsigned id) {
	ext2::INode* socket = get_tmp_inode_at(inode);
	memset(socket, 0, sizeof(ext2::INode));

	socket->type_and_permissions |= TYPE_UNIX_SOCKET;
	socket->size_low = 0;
	socket->os_value_1 = id;

	ext2::write_inode_at(inode, socket);
}

void* ext2::copy_file_to_kmem(ext2::INode* node) {
	unsigned char* data = (unsigned char*) kmalloc(node->size_low);
	ext2::read_file_content(node, 0, data, node->size_low);
	return data;
}