/*
 *  fileHandler.c
 *  MovieStoreServer
 *
 */

#include "fileHandler.h"

unsigned long 
SplitFile( char *path, unsigned long packet_size )
{
	unsigned long size = GetFileSize(path);
	return size / packet_size + 1;
}

unsigned long
GetFileSize(char *path)
{
	struct stat sb;
	stat(path,&sb);
	return sb.st_size;	
}

unsigned long
GetFileData( FILE *fd, unsigned long packet_size, unsigned long index, void **data_ptr )
{
	void *data;
	unsigned long bytes_read;	
	
	if( (data=malloc(packet_size)) == NULL )
		return 0;
	
	fseek(fd,index*packet_size,SEEK_SET);
	
	bytes_read = fread(data,1,packet_size,fd);
	*data_ptr = data;
	
	return bytes_read;	
}

unsigned long
PutFileData( FILE *fd, unsigned long packet_size, unsigned long index, void *data, unsigned long data_size)
{
	unsigned long bytes_written;	
	
	fseek(fd,index*packet_size,SEEK_SET);	
	bytes_written = fwrite(data,1,data_size,fd);
	
	return bytes_written;	
}

boolean 
FileExists( char *path )
{
	FILE * fptr;
	
	if( (fptr=fopen(path,"r")) == NULL ) {
		return FALSE;
	}
	fclose(fptr);
	return TRUE;
}

FILE *
CreateFile(char*path,u_size size)
{
	FILE *out;
	out = fopen(path,"wb+");
	int i;
	
	for(i=0; i<size; i++ ) {
		fputc('x',out);		
	}
	
	return out;
}


