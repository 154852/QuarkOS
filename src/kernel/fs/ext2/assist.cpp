#include <ext2/init.hpp>
#include <ext2/assist.hpp>
#include <string.h>
#include <assertions.h>

bool ext2::new_socket(unsigned node, const char* name, unsigned id) {
	assert(node);

	unsigned newsockid = ext2::allocate_inode();
	assert(newsockid);
	ext2::init_socket(newsockid, id);

	ext2::DirectoryEntry entry;
	entry.name = (char*) name;
	entry.name_len_low = strlen(name);
	entry.type_or_name_len_high = DIRENT_TYPE_SOCKET;
	entry.inode = newsockid;

	ext2::INode* parent = ext2::get_tmp_inode_at(node);
	assert(parent);
	assert(ext2::insert_dirent(&entry, parent));

	return true;
}

bool ext2::mkdir(unsigned node, const char* name) {
	assert(node);

	unsigned newdirid = ext2::allocate_inode();
	assert(newdirid);
	ext2::init_dir_empty(newdirid, node);

	ext2::DirectoryEntry entry;
	entry.name = (char*) name;
	entry.name_len_low = strlen(name);
	entry.type_or_name_len_high = DIRENT_TYPE_DIR;
	entry.inode = newdirid;

	ext2::INode* parent = ext2::get_tmp_inode_at(node);
	assert(parent);
	assert(ext2::insert_dirent(&entry, parent));

	return true;
}

bool ext2::new_text_file(unsigned node, const char* name, const char* string) {
	assert(node);

	unsigned newfileid = ext2::allocate_inode();
	assert(newfileid);
	ext2::init_file_empty(newfileid);
	ext2::INode* file = ext2::get_tmp_inode_at(newfileid);
	int len = strlen(string);
	ext2::write_file_content(newfileid, file, 0, (void*) string, len % 512 == 0? len:(len + (512 - (len % 512))));
	file->size_low = len;
	ext2::write_inode_at(newfileid, file);

	ext2::DirectoryEntry entry;
	entry.name = (char*) name;
	entry.name_len_low = strlen(name);
	entry.type_or_name_len_high = DIRENT_TYPE_FILE;
	entry.inode = newfileid;

	ext2::INode* parent = ext2::get_tmp_inode_at(node);
	assert(parent);
	assert(ext2::insert_dirent(&entry, parent));

	return true;
}