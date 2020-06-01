#pragma once

#include "common.h"

#include "fat.h"

extern struct fat g_fs;

typedef struct {
	uint8_t *fs_bin;
	uint64_t fs_size;
} FAT_UserData;

#define FAT_SECTOR_SIZE 512

typedef struct INode_ {
	// in-memory copy of an inode
	uint64_t dev;  // Device number
	uint64_t inum; // Inode number
	int ref;	   // Reference count

#if 0
	Mutex lock; // protects everything below here
	int valid;          // inode has been read from disk?

	short type;         // copy of disk inode
	short major;
	short minor;
	short nlink;
	uint size;
	uint addrs[NDIRECT+1];
#endif

	struct fat_file file;
} INode;

void fs_init();
error_t fs_open(const char *filename, uint64_t flags, INode **out_opened);
error_t fs_read(INode *inode, char *buf, int buf_len);
