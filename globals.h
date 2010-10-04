#ifndef _GLOBAL_H
#define _GLOBAL_H
#include<sys/stat.h>
struct global_var {
	int fd;
	struct iso_primary_descriptor *pd;
	struct iso_supplementary_descriptor *sd;
	struct iso_directory_record *root_dr;
	int block_size;
	int data_size;
	int block_offset;
	int file_offset;
	int inode_no;
};
struct iso_inode{
	struct iso_directory_record *record;
	struct stat st;
	ino_t st_ino;
	time_t ctime;
	char *sl;
	size_t sl_len;
	char *nm;
	size_t nm_len;
	int cl_block;
	int pl_block;
};
#define ISO_FLAGS_HIDDEN(x) (*((unsigned char *) x) & 1)
#define ISO_FLAGS_DIR(x) (*((unsigned char *) x) & (1 << 1))
#endif
