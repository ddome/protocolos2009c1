/*
 *  app.h
 *  MovieStoreServer
 */

#ifndef __APP_HEADER_H_
#define __APP_HEADER_H_

#include "defines.h"

#define MAX_USER_LEN	50
#define MAX_USER_PASS	50
#define MAX_USER_DESC	300
#define MAX_USER_MAIL	50

#define MAX_MOVIE_LEN	50
#define MAX_MOVIE_GEN	50
#define MAX_MOVIE_PLOT	300
#define M_SIZE          50

#define MAX_PATH_LEN     4096

#define MAX_HOST_LEN 20
#define MAX_PORT_LEN 10

#define MAX_SERVER_LEN 30
#define MAX_SERVER_KEY 10

#define MAX_TICKET_LEN 20

#define _FILE_SIZE_ 100000000L

/* Codigos del pedido (opCode) */

/* Codigos de logueo de usuario */
#define __USER_LOGIN__			0L
#define __USER_ERROR__			1L
#define __PASSWD_ERROR__			2L
#define __LOGIN_OK__				3L
#define __USER_IS_LOG__			4L

/* Codigos de cambio de datos */
#define __NEW_PASSWD__			5L
#define __CHANGE_OK__			6L

/* Codigos de seguridad */
#define __USER_IS_NOT_LOG__		7L
#define __USER_ACCESS_DENY__		8L

/* Codigos de registro de usuario */
#define __REG_USER__				9L
#define __REG_OK__				10L
#define __REG_ERROR__			11L
#define __REG_USER_ERROR__		12L

/* Codigos de logout */
#define __LOG_OUT__				13L
#define __LOG_OUT_ERROR__		14L
#define __LOG_OUT_OK__			15L

/* Codigos de descarga */		
#define __DOWNLOAD__				16L
#define __DOWNLOAD_START__		17L
#define __DOWNLOAD_ERROR__		18L
#define __DOWNLOAD_START_OK__	19L

/* Codigos de comprar pelicula */
#define __BUY_MOVIE__		 20L
#define __BUY_MOVIE_OK__	 21L
#define __BUY_MOVIE_USER_ERROR__ 22L
#define __BUY_MOVIE_PASS_ERROR__ 23L
#define __BUY_MOVIE_ERROR__	 24L
#define __BUY_MOVIE_INVALID__	 25L

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
	char ticket[MAX_TICKET_LEN];
} request_t;

typedef struct {
	char ip[MAX_HOST_LEN];
	char port[MAX_PORT_LEN];
	char ticket[MAX_TICKET_LEN];
} download_start_t;

typedef struct {
	int ret_code;
	char title[MAX_MOVIE_LEN];
	char path[MAX_PATH_LEN];
	unsigned long total_packets;
	u_size size;
} download_header_t;

typedef struct {
	unsigned long n_packet;
	unsigned long total_packets;
	u_size size;
} download_t;

/* Payment servers */

typedef struct {
	char name[MAX_SERVER_LEN];
} server_request_t;

typedef struct {
	char name[MAX_SERVER_LEN];
	char host[MAX_HOST_LEN];
	char port[MAX_PORT_LEN];
	char key[MAX_SERVER_KEY];
} payment_server_t;

/* Compra de peliculas */
typedef struct {
	char movie_name[MAX_MOVIE_LEN];
	char pay_name[MAX_SERVER_LEN];
	char pay_user[MAX_USER_LEN];
	char pay_passwd[MAX_USER_PASS];
} buy_movie_request_t;

typedef struct {
	unsigned long ret_code;
	char ticket[MAX_TICKET_LEN];
} buy_movie_ticket_t;

/* Respuesta */
typedef struct {
	unsigned long ret_code;
} ack_t;

#endif
