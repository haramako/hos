#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "fat.h"

#define PANIC(...)
#define ASSERT_OK(x) assert(x == OK);
#define DBG(fmt, ...) printf(fmt "\n", __VA_ARGS__)

FILE *fd;

void blk_read(size_t sector, void *buf, size_t num_sectors) {
	fseek(fd, sector * 512, SEEK_SET);
	size_t size = fread(buf, num_sectors, 512, fd);
	assert(size == num_sectors * 512);
}

void blk_write(size_t offset, const void *buf, size_t len) { PANIC("NYI"); }

int main(int argc, const char **argv) {

	fd = fopen("test.fat", "r");
	assert(fd);

	struct fat fs;
	error_t err = fat_probe(&fs, blk_read, blk_write);
	assert(err == OK);

	struct fat_dir dir;
	struct fat_dirent *e;
	char tmp[12];
	ASSERT_OK(fat_opendir(&fs, &dir, "/"));
	while ((e = fat_readdir(&fs, &dir)) != NULL) {
		strncpy(tmp, (const char *)e->name, sizeof(tmp));
		tmp[11] = '\0';
		DBG("/%s", tmp);

		char buf[512];
		struct fat_file file;
		err = fat_open(&fs, &file, tmp);
		ASSERT_OK(err);
		err = fat_read(&fs, &file, 0, buf, file.size);
		ASSERT_OK(err);
		buf[file.size] = '\0';
		printf("%s", buf);
	}

	return 0;
}
