/*
 *  fileHandler.h
 *  MovieStoreServer
 *
 */

#ifndef __FILE_HANDLER_H__
#define  __FILE_HANDLER_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include "defines.h"

unsigned long SplitFile( char *path, unsigned long packet_size );

unsigned long GetFileSize(char *path);

unsigned long GetFileData( FILE *fd, unsigned long packet_size, unsigned long index, void **data_ptr );

unsigned long PutFileData( FILE *fd, unsigned long packet_size, unsigned long index, void *data, unsigned long data_size);

boolean FileExists( char *path );

FILE * CreateFile(char*path,u_size size);

char * GetNameFromPath(char * path); 

#endif