#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/statfs.h>
#include<fcntl.h>
#include<errno.h>
#include<fuse.h>
#include<glib.h>
#include"iso_fs.h"
#include"globals.h"
#include"prototypes.h"
struct global_var *gvar; 
static time_t isofs_date(char *stamp, int stamp_len) {
    struct tm tm;
    memset(& tm, 0, sizeof(struct tm));
    
    if(stamp_len == 7) { // ISO9660:9.1.5
        tm.tm_year = stamp[0];
        tm.tm_mon = stamp[1] - 1;
        tm.tm_mday = stamp[2];
        tm.tm_hour = stamp[3];
        tm.tm_min = stamp[4];
        tm.tm_sec = stamp[5];
        tm.tm_isdst = -1;
        tm.tm_gmtoff = stamp[6] * 15 * 60;
    } else if(stamp_len == 17) { // ISO9660:8.4.26.1
        fprintf(stderr, "isofs_date: 17 byte date isn`t supported for the moment, sorry\n");
        return 0;
    } else {
        fprintf(stderr, "isofs_date: unsupported date format, stamp_len %d\n", stamp_len);
        return 0;
    };

    time_t time = mktime(& tm);
    
    return time;
};
static int isofs_direntry2stat(struct stat *st, struct iso_inode *inode) {
    struct iso_directory_record *record = inode->record;
    st->st_ino = inode->st_ino;
    st->st_size = isonum_733(record->size);
    st->st_blocks = st->st_size / gvar->data_size; 
    st->st_blksize = gvar->data_size;
    st->st_nlink = 1; 
    if(ISO_FLAGS_DIR(record->flags)) {
         st->st_mode = S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH | 
         S_IXUSR | S_IXGRP | S_IXOTH; 
     } else {
         st->st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;
     }
        if(!inode->ctime) {
            inode->ctime = isofs_date(record->date, 7);
        };
        
        st->st_atime = inode->ctime;
        st->st_ctime = inode->ctime;
        st->st_mtime = inode->ctime;
    
    return 0;
};
int isofs_getattr(const char *path,struct stat *stbuf)
{
	struct iso_inode *inode = isofs_lookup(path);
	if(!inode) {
		return -ENOENT;
	}
	memset(stbuf,0,sizeof(struct stat));
	isofs_direntry2stat(stbuf,inode);
	return 0;
}
static void read_raw_block(int fd,char *buf,int block_size,int block) 
{
	int n;
	if( lseek(fd,block*block_size,SEEK_SET) == -1) {
		perror("lseek");
		exit(0);
	}
	if((n=read(fd,buf,block_size))<0) {
		perror("read");
		exit(0);
	}
}
int isofs_opendir(const char *path) 
{
struct iso_inode *inode = isofs_lookup(path);
if(!inode) {
	fprintf(stderr,"no such path");
	return ENOENT;
}
if(!ISO_FLAGS_DIR(inode->record->flags)) {
	return -ENOTDIR;
}
	return 0;
}
int isofs_readdir(const char *path,void *buf,isofs_dir_fill_t filler)
{
	int count = 0;
	log_msg("\nisofs_readdir(path=\"%s\",buf=0x%080x,filler=0x%08x)\n",path,buf,filler);
	struct iso_inode *inode = isofs_lookup(path);
	if(!inode) {
		fprintf(stderr,"nothing to be done");
		return ENOENT;
	}
	struct iso_directory_record *gvar1 = inode->record;
	int extent = isonum_733(gvar1->extent);
	int pdir_size = isonum_733(gvar1->size);
	int fd = gvar -> fd;
	int block_size = gvar->block_size;
	int record_length = 0,record_name_length = 0,pad_len = 0,sa_len = 0;
	char *entry = 0;
	char buf1[block_size];
	int block_count = (pdir_size/block_size);
	int block = extent;
	while(block_count) {
	int boff = 0;
	struct iso_directory_record *dr = 0;
	memset(buf1,0,block_size);
	read_raw_block(fd,buf1,block_size,block);
	do {
		 dr = (struct iso_directory_record *) (buf1 + boff);
		 record_length = isonum_711(dr->length);
		 record_name_length = isonum_711(dr->name_len);
		 pad_len = ((record_name_length & 1) ? 0:1);
		 sa_len = record_length - record_name_length - sizeof(struct iso_directory_record) - pad_len;
		if(record_length == 0)
			break;
		struct iso_inode *ino = (struct iso_inode *) malloc(sizeof(struct iso_inode));
		if(!ino) {
			fprintf(stderr,"cannot mallock \n");
			exit(ENOENT);
		} else {
			memset(ino,0,sizeof(struct iso_inode));
		}	
		struct iso_directory_record *dr_new = (struct iso_directory_record *) malloc(sizeof(struct iso_directory_record));
		if(!dr_new) {
			fprintf(stderr,"cannot mallock\n");
			exit(ENOENT);
		}
		memcpy(dr_new,dr,sizeof(struct iso_directory_record));
		ino->record = dr_new;
		log_msg("ino->record 0x%08x \n",ino->record);
		ino->st_ino = gvar->inode_no;
		gvar->inode_no++;
		if(count == 0) {
			entry = (char *) malloc(2);
			strcpy(entry,".");
		} else if(count == 1) {
			entry = (char *) malloc(3);
			strcpy(entry,"..");
		} else {
		entry = (char *) malloc(record_name_length+1);
		memset(entry,0,record_name_length+1);
		strncpy(entry,dr->name,record_name_length);
		entry[record_name_length]=0;
		}
		log_msg("calling filler with name %s\n",entry);
		//if(count > 1 && entry !=0 )
		if(filler) {
		//struct stat *st = (struct stat *) malloc(sizeof(struct stat));
		struct stat st;
		memset(&st,0,sizeof(struct stat));
		isofs_direntry2stat(&st, ino);
		filler(buf,entry,&st,0);
		}
		char *real_path = (char *) malloc(strlen(path)+record_name_length+1+2);
		strcpy(real_path,path);
		if(*(real_path+1)!=0) {
			strcat(real_path,"/");
		}
		strcat(real_path,entry);
		log_msg("printing real_path:%s\n",real_path);
		if(g_hash_table_lookup(lookup_table,real_path)) {
			free(ino);
		} else {
		log_msg("calling g_hash_table with %s inode record 0x%08x\n",realpath,ino->record);
		g_hash_table_insert(lookup_table,g_strdup(real_path),ino);
		}
		free(entry);
		boff +=record_length;
		count++;
	}while( count == 1 || count == 2 ||  (record_name_length !=1 ) && (dr->name[0] !=0) );
	block_count--;
	block++;
	}
	return 0;
}
int isofs_statfs(struct statfs *stbuf) 
{
	stbuf->f_type = ISOFS_SUPER_MAGIC;
	stbuf->f_bsize = gvar->block_size;
	stbuf->f_blocks = 0;
	stbuf->f_bfree = 0;
	stbuf->f_bavail = 0;
	stbuf->f_files = 0;
	stbuf->f_ffree = 0;
	stbuf->f_namelen = 25;
	return 0;
}
