#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include"iso_fs.h"
#include"globals.h"
#include"prototypes.h"
int isofs_open(const char *path)
{
	struct iso_inode *inode = isofs_lookup(path);
	if(!inode) {
		fprintf(stderr,"no such file or directory found \n");
		return -ENOENT;
	}
	if(ISO_FLAGS_DIR(inode->record->flags)) {
		fprintf(stderr,"not a regular file may be a directory\n");
		return -EINVAL;
	}
	return 0;
}
static int read_raw_block(int block,char *buf)
{
	int n;
	if(buf == 0) {
		fprintf(stderr,"cannot fill this block");
		exit(0);
	}
	if(lseek(gvar->fd,block*gvar->block_size,SEEK_SET) == -1) {
		perror("cannot seek\n");
		exit(0);
	}
	if((n = read(gvar->fd,buf,gvar->block_size))<0) {
		perror("cannot print this block");
		exit(0);
	}
	return n;
}
int isofs_read(const char *path,char *read_buf,size_t size,off_t offset)
{
	log_msg("i am now in read_block\n");
	struct iso_inode *inode = isofs_lookup(path);
	if(!inode) {
		fprintf(stderr,"no such file or directory found \n");
		return -ENOENT;
	}
	if(ISO_FLAGS_DIR(inode->record->flags)) {
		fprintf(stderr,"not a regular file may be a directory\n");
		return -EINVAL;
	}
	int data_size = gvar->data_size;
	struct iso_directory_record *dr = inode->record;
	int start_block = isonum_733(dr->extent);
	int end_block = isonum_733(dr->size)/data_size;
	int in_block = end_block - start_block ;
	
	int offset_block_start = offset / data_size;
	int offset_block_end = (offset + size) / data_size;
	int offset_shift_point = (offset%data_size);
	int block = offset_block_start + start_block;
	log_msg("malloced .....\n");
	char *buf = (char *) malloc(data_size);
	if(!buf) {
		perror("isofs_read:cannot malloc");
		exit(0);
	}
	int total = 0;
	int count = 0;
	int len;
	do{
		log_msg("read_raw_block.....\n");
		len = read_raw_block(block,buf);
		log_msg("buffer is now ....%s\n",buf);
		memcpy(read_buf+count*data_size,buf+offset_shift_point,data_size);
		log_msg("the read_buf is %s",read_buf);
		offset_shift_point = 0;
		count ++;
		block++;
		total += len;
	}while(count <= in_block);
	
	return total;
}
