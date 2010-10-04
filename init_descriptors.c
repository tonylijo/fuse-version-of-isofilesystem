#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdio.h>
#include"iso_fs.h"
#include<errno.h>
#include"globals.h"
void init_descriptors(struct global_var *gvar) 
{
	int vd_type,n;
	struct iso_volume_descriptor *vd = (struct iso_volume_descriptor *) malloc(sizeof(struct iso_volume_descriptor));
	int fd = gvar->fd;
	/*iso filesystem default values*/
	int block_size = 2048;
	int data_size = 2048;
	int block_offset = 0;
	int file_offset = 0;
	
	char buf[block_size];

	if(lseek(fd,16*block_size,SEEK_SET) == -1) {
		perror("seek error");
		exit(0);
	}

	if((n = read(fd,buf,block_size)) < 0) {
		perror("read error");
		exit(0);
	}
	
	memset(vd,0,sizeof(struct iso_volume_descriptor));
	memcpy(vd,buf,sizeof(struct iso_volume_descriptor));
	
	int end = 0;
	
	if(strncmp(vd->id,"CD001",5) != 0) {
		fprintf(stderr,"no iso volume identifier CD001 found in desired position .....\n");
		exit(0);
	}
	else {
		do {
			vd_type = isonum_711(vd->type);
			switch(vd_type) {
				case ISO_VD_PRIMARY:
					if(gvar->pd != 0) {
						fprintf(stderr,"iso primary volume descriptor already found....");
						exit(0);
					}
					else {
						fprintf(stderr,"primary volume discriptor found. now copying values.......\n");
						gvar->pd = (struct iso_primary_descriptor *) malloc(sizeof(struct iso_primary_descriptor));
						memset(gvar->pd,0,sizeof(struct iso_primary_descriptor));
						memcpy(gvar->pd,vd,sizeof(struct iso_primary_descriptor));
					}
					break;
				case ISO_VD_SUPPLEMENTARY :
					if(gvar->pd == 0) {
						fprintf(stderr,"iso primary volume descriptor not found yet and supplimentary descriptor is been found\n");
						exit(0);
					}
					else {
						fprintf(stderr,"supplementary volume descriptor found. now copying values......\n");
						gvar->sd = (struct iso_supplementary_descriptor *) malloc(sizeof(struct iso_supplementary_descriptor));
						memset(gvar->sd,0,sizeof(struct iso_supplementary_descriptor));
						memcpy(gvar->sd,vd,sizeof(struct iso_supplementary_descriptor));
					}
					break;
				case ISO_VD_END:
					if(gvar->pd == 0) {
						fprintf(stderr,"iso primary volume descriptor still not found and end volume descriptor reached \n");
						exit(0);
					}
					fprintf(stderr,"volume descriptor end found now exiting......\n");
					end = 1;
					break;
					}
			if(!end) {
			
				if((n = read(fd,buf,block_size)) < 0) {
					perror("read error");
					exit(0);
				}

				memset(vd,0,sizeof(struct iso_volume_descriptor));
				memcpy(vd,buf,sizeof(struct iso_volume_descriptor));

				if(strncmp(vd->id,"CD001",5) != 0) {
					fprintf(stderr,"no iso volume identifier CD001 found in desired position .....\n");
					exit(0);
				}
			}
		} while(!end);
	}
	gvar->root_dr = (struct iso_directory_record *) (gvar->pd)->root_directory_record;
	gvar->block_size = block_size;
	gvar->data_size = data_size;
	gvar->block_offset = block_offset;
	gvar->file_offset = file_offset;
	free(vd);
	//free(buf);

}
