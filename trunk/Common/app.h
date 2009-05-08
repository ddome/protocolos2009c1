/*
 *  app.h
 *  MovieStoreServer
 */

#ifndef __APP_HEADER_H_
#define __APP_HEADER_H_

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

#endif