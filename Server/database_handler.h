/*
 *  database_handler.h
 *  MovieStoreServer
 *
 *  Created by Damian Dome on 6/6/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __DATABASE_H__
#define  __DATABASE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../Common/app.h"

/* Estructura privada con la informacion de los tickets asociados a una descarga */
typedef struct {
	char ticket[MAX_TICKET_LEN];
	char path[MAX_PATH_LEN];
	char MD5[M_SIZE];
	unsigned char n_downloads;
} ticket_info_t;

/* Estructura privada con la informacion de los archivos */
typedef struct {
	char name[MAX_MOVIE_LEN];
	char path[MAX_PATH_LEN];
	char MD5[M_SIZE];
	unsigned char n_downloads;
} file_info_t;

int TicketInfoComp( void * v1, void *v2 );

int TicketHash( void *v, int size );

int TicketSave(FILE *fd,void *data);

void * TicketLoad(FILE *fd);

int UsersComp( void *v1, void *v2 );

int UsersHash( void *v1, int size );

#endif