/*
 *  app.h
 *  MovieStoreServer
 */

#ifndef __APP_HEADER_H_
#define __APP_HEADER_H_

#include "defines.h"

#define MAX_USER_LEN  50
#define MAX_USER_PASS 50
#define MAX_USER_DESC 300
#define MAX_USER_MAIL 50

#define MAX_MOVIE_LEN 50
#define MAX_MOVIE_GEN 50
#define MAX_MOVIE_PLOT 300
#define M_SIZE 50

/* Codigos del pedido (opCode) */

/* Codigos de logueo de usuario */
#define __USER_LOGIN__             0L
#define __USER_ERROR__             1L
#define __PASSWD_ERROR__           2L
#define __LOGIN_OK__               3L
#define __USER_IS_LOG__            4L

/* Codigos de cambio de datos */
#define __NEW_PASSWD__             5L
#define __CHANGE_OK__			   6L

/* Codigos de seguridad */
#define __USER_IS_NOT_LOG__        7L
#define __USER_ACCESS_DENY__	   8L

/* Codigos de registro de usuario */
#define __REG_USER__			   9L
#define __REG_OK__				  10L
#define __REG_ERROR__			  11L
#define __REG_USER_ERROR__		  12L


/* Estructuras del protocolo de aplicacion */

/* Header */
typedef struct {
	unsigned long int opCode;
	unsigned long int total_objects;
	char user[MAX_USER_LEN]; /* 'anonimo' para usuarios anonimos */
	char passwd[MAX_USER_PASS]; /* 'anonimo' para usuarios anonimos */
} header_t;

/* User Login */
typedef struct login {
	char user[MAX_USER_LEN];
	char passwd[MAX_USER_PASS];
} login_t;

/* Registrar usuario */
typedef struct {
	char user[MAX_USER_LEN];
	char passwd[MAX_USER_PASS];
	char desc[MAX_USER_DESC];
	char mail[MAX_USER_MAIL];
	unsigned char level;
} client_t;

/* Pelicula */
typedef struct {
	char name[MAX_MOVIE_LEN];
	char gen[MAX_MOVIE_GEN];
	char plot[MAX_MOVIE_PLOT];
	u_size duration; /* En minutos */
	u_size size;     /* En bytes   */ 
	u_size value;
	char MD5[M_SIZE];
} movie_t;

/* Descarga */
typedef struct {
	unsigned long n_packet;
	unsigned long total_packets;
	u_size size;
} download_t;

/* Respuesta */
typedef struct {
	unsigned long ret_code;
} ack_t;

#endif