#ifndef _EXT2FS_PATH_H
#define _EXT2FS_PATH_H

#include <ext2/init.hpp>

namespace ext2 {
	INode* inode_from_path(const char* path, INode* start);
	INode* inode_from_relative_path(const char* path, INode* dir);
	INode* inode_from_root_path(const char* path);

	unsigned inode_id_from_path(const char* path, INode* start);
	unsigned inode_id_from_relative_path(const char* path, INode* dir);
	unsigned inode_id_from_root_path(const char* path);
};

#endif