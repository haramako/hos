#include "fs.h"

#include "boot_param.h"
#include "fat.h"

struct fat g_fs;

static INode root_;

#define INODES_LEN 256
static INode inodes_[INODES_LEN];

static void blk_read_(void *data, size_t sector, void *buf, size_t num_sectors) {
	FAT_UserData *d = (FAT_UserData *)data;
	memcpy(buf, d->fs_bin + sector * FAT_SECTOR_SIZE, num_sectors * FAT_SECTOR_SIZE);
}

static void blk_write_(void *data, size_t offset, const void *buf, size_t len) { kpanic("NYI"); }

void fs_init() {
	FAT_UserData *data = malloc(sizeof(FAT_UserData));
	data->fs_bin = g_boot_param->bootfs_buf;
	data->fs_size = g_boot_param->bootfs_size;

	error_t err = fat_probe(&g_fs, blk_read_, blk_write_, data);
	assert(err == OK);

	memset(&root_, 0, sizeof(root_));
}

INode *fs_find_inode(uint64_t inum) {
	for (int i = 0; i < INODES_LEN; i++) {
		if (inodes_[i].inum == inum) {
			return &inodes_[i];
		}
	}
	return NULL;
}

INode *make_inode_(uint64_t inum) {
	for (int i = 0; i < INODES_LEN; i++) {
		if (inodes_[i].inum == 0) {
			inodes_[i].inum = inum;
			return &inodes_[i];
		}
	}
	kpanic("No inode left.");
}

void fat_name_(char *buf, const char *filename) {
	memset(buf, ' ', 11);
	buf[11] = '\0';

	for (int i = 0, o = 0;; i++, o++) {
		if (filename[i] == '.') {
			o = 8;
		} else {
			buf[o] = filename[i];
		}
		kcheck(o >= 11, "Invalid fat name");
	}
}

INode *fs_root() { return &root_; }

error_t fs_open(const char *filename, uint64_t flags, INode **out_opened) {
	struct fat_file file;
	error_t err = fat_open(&g_fs, &file, filename);
	if (err != OK) {
		*out_opened = NULL;
		return ERR_NOT_FOUND;
	}

	INode *inode = make_inode_(file.cluster);
	inode->file = file;

	*out_opened = inode;
	return ERR_OK;
}

error_t fs_read(INode *inode, char *buf, size_t offset, size_t *io_buf_len) {
	kcheck0(io_buf_len);
	kcheck0(inode->inum != 0);

	error_t err = fat_read(&g_fs, &inode->file, offset, buf, *io_buf_len);
	if (err != OK) {
		ERR_UNKNOWN;
	}
	return ERR_OK;
}
