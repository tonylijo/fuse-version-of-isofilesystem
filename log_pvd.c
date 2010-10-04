#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include"iso_fs.h"
char *print_signed_char(char *ptr,int n_len)
{
	int n_loop;
	char *tmpstr = (char *) malloc(n_len+1);
	if(n_len>0) {
		for(n_loop=0;n_loop<n_len;n_loop++) {
			*(tmpstr + n_loop) = (unsigned char) *(ptr+n_loop);
		}
		*(tmpstr + n_loop) = 0;
		return tmpstr;
	}
	else {
		return 0;
	}
}
display_dir_info(struct iso_directory_record *dr,FILE *fp)
{
	fprintf(fp,"LENGTH\t:%d\n",isonum_711(dr->length));
	fprintf(fp,"EXTENDED ATTRIBUTE LENGTH\t:%d\n",isonum_711(dr->ext_attr_length));
	fprintf(fp,"EXTENT\t:%d\n",isonum_733(dr->extent));
	fprintf(fp,"SIZE\t:%d\n",isonum_733(dr->size));
	fprintf(fp,"DATE\t:\n");
	fprintf(fp,"FLAGS\t:%d\n",isonum_711(dr->flags));
	fprintf(fp,"FILE UNIT SIZE\t:%d\n",isonum_711(dr->file_unit_size));
	fprintf(fp,"INTERLEAVE\t:%d\n",isonum_711(dr->interleave));
	fprintf(fp,"VOLUME SEQUENCE NUMBER\t:%d\n",isonum_723(dr->volume_sequence_number));
	//fprintf(fp,"NAME\t:");print_signed_char(dr->name,isonum_711(dr->name_len));printf("\n");
}
void read_dir_info(char *p,FILE *fp)
{
struct iso_directory_record dr;
memset(&dr,0,sizeof(struct iso_directory_record));
memmove(&dr,p,sizeof(struct iso_directory_record));
display_dir_info(&dr,fp);
}
void display_pvd_info(struct iso_primary_descriptor *pd,FILE *fp) 
{
	fprintf(fp,"TYPE\t\t:%d\n",pd->type[0]);
	fprintf(fp,"ID\t\t:%s",print_signed_char(pd->id,5));
	fprintf(fp,"VERSION\t\t:%d\n",(*(pd->version)));
	fprintf(fp,"SYSTEM ID\t: %s\n",print_signed_char((pd->system_id),32));
	fprintf(fp,"VOLUME ID\t:%s\n",print_signed_char((pd->volume_id),32));
	fprintf(fp,"VOLUME SPACE\t: %ld\n",isonum_733(pd->volume_set_size));
	fprintf(fp,"VOLUME SET\t:%d\n",isonum_723(pd->volume_set_size));
	fprintf(fp,"VOLUME NUMBER\t:%d\n",isonum_723(pd->volume_sequence_number));
	fprintf(fp,"LOGICAL BLOCK SIZE\t:%d\n",isonum_723(pd->logical_block_size));
	fprintf(fp,"PATH TABLE SIZE\t:%ld\n",isonum_733(pd->path_table_size));
	fprintf(fp,"PATH TABLE L\t:%ld\n",isonum_731(pd->type_l_path_table));
	fprintf(fp,"TYPE TABLE L\t:%ld\n",isonum_731(pd->opt_type_l_path_table));
	fprintf(fp,"PATH TABLE M\t:%ld\n",isonum_732(pd->type_m_path_table));
	fprintf(fp,"TYPE TABLE M\t:%ld\n",isonum_732(pd->opt_type_m_path_table));
	//fprintf(fp,"ROOT DIR\t:%ld\n",(*(pd->root_directory_record)));
	read_dir_info(pd->root_directory_record,fp);
	fprintf(fp,"VOLUME SET ID\t:%s\n",print_signed_char((pd->volume_set_id),128));
	fprintf(fp,"PUBLISHER\t:%s\n",print_signed_char((pd->publisher_id),128));
	fprintf(fp,"PREPARED\t:%s\n",print_signed_char((pd->preparer_id),32));
	fprintf(fp,"APPLICATION\t:%s\n",print_signed_char((pd->application_id),128));
	fprintf(fp,"COPYRIGHT\t:%s\n",print_signed_char((pd->copyright_file_id),37));
	fprintf(fp,"ABSTRACT\t:%s\n",print_signed_char((pd->abstract_file_id),37));
	fprintf(fp,"CREATED\t:%s\n",print_signed_char((pd->creation_date),17));
}
void print_primary_values(struct iso_primary_descriptor *pd)
{
	FILE *fp;	
	char buf[2048];
	if( ( fp = fopen("pvd_logfile.log","w") ) == 0 ) {
		perror("open");
		exit(0);
	}
	display_pvd_info(pd,fp);
}
