/*
 *  defines.h
 *  MovieStoreServer
 */

/* Aclaracion: cualquier estado de error definirlo como menor a 0 */


#ifndef __DEFINES_H_
#define __DEFINES_H_

/* DEFINES DE USO GENERAL */

typedef enum status { FATAL_ERROR=-2, ERROR=-1, OK=1 } status;

typedef enum boolean { FALSE=0, TRUE=1 } boolean;

typedef unsigned long u_size;

typedef char * string;

/* ESTRUCTURAS DE APP */

typedef struct {
	char name[50];
	char gen[50];
	char plot[300];
	u_size duration; /* En minutos */
	u_size size;     /* En bytes   */ 
	u_size value;
	u_size MD5; /*?????*/
} movie_t;

typedef struct {
	char user[50];
	char passwd[50];
	char desc[300];
	char mail[50];
	unsigned char level;
} client_t;


/* DEFINES DE TRANSPORT */

/* DEFINES DE DATABASE */

#endif



