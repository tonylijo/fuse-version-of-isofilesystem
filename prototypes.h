#ifndef _PROTOTYPES_H
#define _PROTOTYPES_H
#include<fuse.h>
#include<glib.h>
#include"globals.h"
#include<sys/statfs.h>
GHashTable *lookup_table ;
GHashTable *negative_lookup_table;
typedef int (*isofs_dir_fill_t) (void *buf, const char *name,const struct stat *stat, off_t off);
struct global_var *gvar;
void init_descriptors(struct global_var *);
void print_primary_values(struct iso_primary_descriptor *);
void print_supplementary_vlaues(struct iso_supplementary_descriptor *);
int isofs_readdir(const char *,void *,fuse_fill_dir_t);
struct iso_inode *isofs_lookup(const char *path);
int isofs_statfs(struct statfs *stbuf);
// int isofs_direntry2stat(struct stat *st, isofs_inode *inode); 
int isofs_getattr(const char *path,struct stat *stbuf);
int isofs_read(const char *path,char *buf,size_t size,off_t offset);
int isofs_open(const char *path);
#endif
