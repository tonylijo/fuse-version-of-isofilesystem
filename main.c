#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/statfs.h>
#include<fcntl.h>
#include<string.h>
#include<errno.h>
#include<glib.h>
#include<fuse.h>
#include"iso_fs.h"
#include"globals.h"
#include"prototypes.h"
#include"log.h"
#include"params.h"
static int iso_getattr(const char *path,struct stat *stbuf)
{
	return isofs_getattr(path,stbuf);
}
static int iso_opendir(const char *path,struct fuse_file_info *fi)
{
return isofs_opendir(path);
}
static int iso_readdir(const char *path,void *buf,fuse_fill_dir_t filler,off_t offset,struct fuse_file_info *fi)
{
	log_msg("\n main.c iso_readdir(path=\"%s\",buf=0x%080x,filler=0x%08x)\n",path,buf,filler);
	return isofs_readdir(path,buf,filler);
}
static int iso_statfs(const char *path,struct statfs *stbuf)
{
	return isofs_statfs(stbuf);
}
static int iso_open(const char *path,struct fuse_file_info *fi) 
{
	return isofs_open(path);
}
static int iso_read(const char *path,char *buf,size_t size,off_t offset,struct fuse_file_info *fi) 
{
	return isofs_read(path,buf,size,offset);
}
static struct fuse_operations iso_oper = {
	.getattr = iso_getattr,
	.opendir = iso_opendir,
	.readdir = iso_readdir,
	.open = iso_open,
	.read = iso_read,
	.statfs = iso_statfs,
};
int main(int argc,char *argv[])
{
	int fd;
	//int fuse_stat;
	
	struct bb_state *bb_data;
	bb_data = calloc(sizeof(struct bb_state),1);
	if(bb_data == NULL) {
		perror("main calloc");
		abort();
	}
	bb_data->logfile = log_open();
	if((fd = open(argv[2],O_RDONLY)) < 0) {
		perror("main->open");
		exit(0);
	}
	gvar = (struct global_var *) malloc(sizeof(struct global_var));
	gvar->fd = fd;
	init_descriptors(gvar);
	argv[2]=0;
	--argc;
	lookup_table = g_hash_table_new(g_str_hash,g_str_equal);
	negative_lookup_table = g_hash_table_new(g_str_hash,g_str_equal);
	struct iso_inode *inode  = (struct iso_inode *) malloc(sizeof(struct iso_inode));
	if(!inode) {
		perror("can't malloc:");
		exit(ENOMEM);
	}
	memset(inode,0,sizeof(struct iso_inode));
	inode->record = gvar->root_dr;
	gvar->inode_no = 1;
	inode->st_ino = gvar->inode_no;
	gvar->inode_no++;
	g_hash_table_insert(lookup_table,"/",inode);
	
	return fuse_main(argc,argv,&iso_oper,bb_data);
}
