/* This file contains the ecma standerd defanitions */
#include<stdio.h>

int isonum_711(char *p) 
{
	return ( *(unsigned char *) p );
}

int isonum_721(char *p) 
{
	return( ( p[0] & 0xff ) | ( ( p[1] & 0xff ) << 8 ) );
}

int isonum_723(char *p)
{
	if( p[0] != p[3] || p[1] != p[2] ) {
		fprintf(stderr,"invalid format 723 number \n ");
		return -1;
	}
	return ( isonum_721 ( p ) );
}

int  isonum_731(char *p)
{
	return ( ( p[0] & 0xff )
		| ( ( p[1] & 0xff ) << 8 )
		| ( ( p[2] & 0xff ) << 16 )
		| ( ( p[3] & 0xff ) << 24 ) );
}

int isonum_732(char *p) 
{
	return ( ( p[3] & 0xff )
		| ( ( p[2] & 0xff ) << 8 )
		| ( ( p[1] & 0xff ) << 16 )
		| ( ( p[0] & 0xff ) << 24 ) );
}

int isonum_733(char *p) 
{
	if( p[0] != p[7] || p[1] != p[6] || p[2] != p[5] || p[3] != p[4] ) {
		fprintf(stderr,"invalic format 733 number \n");
		return -1;
	}

	return ( isonum_731 ( p ) );
}
