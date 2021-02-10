#ifndef _EXT2FS_INIT_H
#define _EXT2FS_INIT_H

#define FS_CLEAN 1
#define FS_ERRORS 2

#define ERR_HANDLE_IGNORE 1
#define ERR_HANDLE_READ_ONLY 2
#define ERR_HANDLE_PANIC 3

#define TYPE_FIFO 0x1000
#define TYPE_CHARACTER_DEVICE 0x2000
#define TYPE_DIRECTORY 0x4000
#define TYPE_BLOCK_DEVICE 0x6000
#define TYPE_FILE 0x8000
#define TYPE_SYM_LINK 0xA000
#define TYPE_UNIX_SOCKET 0xC000

#define PERMISSIONS_OTHER_EXEC 0x001
#define PERMISSIONS_OTHER_WRITE 0x002
#define PERMISSIONS_OTHER_READ 0x004
#define PERMISSIONS_GROUP_EXEC 0x008
#define PERMISSIONS_GROUP_WRITE 0x0010
#define PERMISSIONS_GROUP_READ 0x0020
#define PERMISSIONS_USER_EXEC 0x0040
#define PERMISSIONS_USER_WRITE 0x0080
#define PERMISSIONS_USER_READ 0x0100
#define PERMISSIONS_STICLY 0x0200
#define PERMISSIONS_SET_GID 0x0400
#define PERMISSIONS_SET_UID 0x0400

#define FLAG_SECURE_DELETION 0x1
#define FLAG_KEEP_ON_DELETE 0x2
#define FLAG_COMPRESSION 0x4
#define FLAG_SYNC_UPDATES 0x8
#define FLAG_IMMUTABLE 0x10
#define FLAG_APPEND_ONLY 0x20
#define FLAG_NOT_IN_DUMP 0x40
#define FLAG_NO_TIME_UPDATE 0x80

#define DIRENT_TYPE_UNKNOWN 0
#define DIRENT_TYPE_FILE 1
#define DIRENT_TYPE_DIR 2
#define DIRENT_TYPE_CHAR_DEVICE 3
#define DIRENT_TYPE_BLOCK_DEVICE 4
#define DIRENT_TYPE_FIFO 5
#define DIRENT_TYPE_SOCKET 6
#define DIRENT_TYPE_SYMBOLIC_LINK 7

namespace ext2 {
	struct SuperBlock {
		unsigned inode_count;
		unsigned block_count;
		unsigned superuser_blocks;
		unsigned unallocated_blocks;
		unsigned unallocated_inodes;

		unsigned superblock_block_idx;
		
		unsigned block_size;
		unsigned fragment_size;
		
		unsigned blocks_per_block_group;
		unsigned fragments_per_block_group;
		unsigned inodes_per_block_group;
		
		unsigned last_mount_time;
		unsigned last_write_time;
		
		unsigned short mount_count_since_fsck;
		unsigned short max_mounts_to_fsck;

		unsigned short signature;

		unsigned short state; // FS_CLEAN / FS_ERRORS
		unsigned short error_action; // ERR_HANDLE_IGNORE ... ERR_HANDLE_PANIC

		unsigned short version_minor;
		
		unsigned last_fsck_time;
		unsigned fsck_time_interval;

		unsigned last_os_id;

		unsigned version_major;

		unsigned short reserved_uid;
		unsigned short reserved_gid;
	};

	struct BlockGroupDescriptor {
		unsigned block_usage_bitmap_addr;
		unsigned inode_usage_bitmap_addr;
		unsigned inode_table_addr;
		unsigned short unallocated_blocks_in_group;
		unsigned short unallocated_nodes_in_group;
		unsigned short directories_in_group;
	};

	struct INode {
		unsigned short type_and_permissions;
		unsigned short userid;
		unsigned size_low;
		
		unsigned last_access_time;
		unsigned creation_time;
		unsigned modification_time;
		unsigned deletion_time;
		
		unsigned short groupid;
		unsigned short hard_links;
		unsigned disk_sectors;
		unsigned flags;
		unsigned os_value_1;
		
		unsigned direct_block_pointers[12];
		unsigned singly_indirect_block_pointer;
		unsigned doubly_indirect_block_pointer;
		unsigned triply_indirect_block_pointer;
		
		unsigned generation_number;
		unsigned reserved0;
		unsigned reserved1;
		unsigned fragment_block;
		
		char os_value_2[12];
	};

	struct DirectoryEntry {
		unsigned inode;
		unsigned short size;
		unsigned char name_len_low;
		unsigned char type_or_name_len_high; // or 
		char* name;
	};

	void init();

	unsigned block_group_for(unsigned inode);
	unsigned inode_in_group_for(unsigned inode);

	void read_raw(unsigned addr, void* out, unsigned length);
	void write_raw(unsigned addr, void* data, unsigned length);
	void read_file_content(INode* node, unsigned off, void* out, unsigned length);
	void* copy_file_to_kmem(INode* node);

	BlockGroupDescriptor* block_group_descriptor_at(unsigned index);
	INode* get_tmp_inode_at(unsigned index);
	INode* get_tmp_root_inode();
	
	DirectoryEntry* read_tmp_direntry(INode* node, unsigned idx);
	DirectoryEntry* find_tmp_direntry(INode* node, const char* name);
	DirectoryEntry* find_tmp_direntryl(INode* node, const char* name, unsigned len);
	unsigned dir_entry_count(INode* node);

	bool is_dir(INode* node);

	unsigned allocate_block();
};

#endif