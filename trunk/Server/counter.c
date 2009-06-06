/*
 *  counter.c
 *  MovieStoreServer
 *
 *  Created by Damian Dome on 6/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "counter.h"
#include <stdio.h>
#include <stdlib.h>

COUNTER 
LoadCounter(char *path)
{
	COUNTER aux;
	FILE *fd;
	
	if( (fd=fopen(path,"rb")) == NULL )
		return 0;
	
	if( (fread(&aux,sizeof(unsigned long),1,fd) < 1 ) )
		return 0;
	
	fclose(fd);
	return aux;
}

int 
SaveCounter(COUNTER aux,char *path)
{
	FILE *fd;
	
	if( (fd=fopen(path,"wb+")) == NULL )
		return -1;
	
	if( (fwrite(&aux,sizeof(unsigned long),1,fd) < 1 ) )
		return -1;
	
	fclose(fd);
	return 1;
}
