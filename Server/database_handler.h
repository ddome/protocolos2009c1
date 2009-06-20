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

#include "movieDB.h"
#include "hashADT.h"
#include "../Common/app.h"

/* Estructura con la informacion de los tickets asociados a una descarga */
typedef struct {
	char ticket[MAX_TICKET_LEN];
	char path[MAX_PATH_LEN];
	char MD5[M_SIZE];
	unsigned char n_downloads;
} ticket_info_t;

/* Estructura con la informacion de los archivos */
typedef struct {
	char name[MAX_MOVIE_LEN];
	char path[MAX_PATH_LEN];
	char MD5[M_SIZE];
	float value;
} file_info_t;

/* Usuarios conectados */

int UsersComp( void *v1, void *v2 );

int UsersHash( void *v1, int size );

/* Tickets generados */

int TicketInfoComp( void * v1, void *v2 );

int TicketHash( void *v, int size );

int TicketSave(FILE *fd,void *data);

void * TicketLoad(FILE *fd);

/* Informacion de los archivos del sistema */

void * FileInfoLoad(FILE *fd);

int FileInfoSave(FILE *fd,void *data);

int FileInfoHash( void *v, int size );

int FileInfoComp( void * v1, void *v2 );

/* Manejo de las peliculas disponibles */

int InitDB(dbADT db,char * pathName,hashADT files_info);

#endif
