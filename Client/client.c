/*
 *  client.c
 *  MovieStoreClient
 *
 *  Created by Damian Dome on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"
#include "Prompt.h"
#include "../Common/des/include/encrypt.h"
#include "../Common/fileHandler.h"

char log_user[MAX_USER_LEN];
char log_passwd[MAX_USER_PASS];

/* Static Functions */

static unsigned long SendRequest(u_size op_code,u_size total_objects,void *packet, u_size size);

static download_header_t SendDownloadRequest(void *packet, u_size size);

static status StartDownload(FILE *fd);

static status ListenMovie(FILE *fd,char *port);

static status ProcessDownload(void *packet);

status 
InitClient(void)
{
	/* Cliente por default, sin privilegios */
	strcpy(log_user, "anonimo");
	strcpy(log_passwd, "anonimo");
	return OK;
}

status
StartClient(void)
{	
	Prompt();
	return OK;
}

void
EndClient(void)
{
	return;
}

void 
UserExit(void)
{
	if( strcmp(log_user, "anonimo") != 0 )
		UserLogout();
}


client_login_status
UserLogin(char *user, char* passwd)
{

	int ret;
	int ret_code;
	login_t log;
	
	/* Si estaba logueado con otra cuenta, lo deslogueo */
	if( strcmp(log_user, "anonimo") != 0 )
		UserLogout();
	
	/* Paquete del pedido */
	strcpy(log.user,user);
	strcpy(log.passwd,passwd);
	/* Mando el paquete */
	ret_code = SendRequest(__USER_LOGIN__, 1, &log, sizeof(login_t));
	/* Proceso la respuesta */
	switch (ret_code) {
		case __LOGIN_OK__: 
		case __USER_IS_LOG__:
			/* Guardo los datos del usuario */
			strcpy(log_user,log.user);
			strcpy(log_passwd,log.passwd);
			ret = USER_LOGIN_OK;
			break;
		case __USER_ERROR__:
			ret = LOGIN_USER_INVALID;
			break;
		case __PASSWD_ERROR__:
			ret = LOGIN_PASS_INVALID;
			break;
		default:
			ret = LOGIN_CONNECT_ERROR;
			break;
	}
		
	return ret;
}

client_change_passwd_status
UserChangePasswd(char *new_passwd, char *rep_new_passwd)
{
	client_change_passwd_status ret;
	login_t new_client_info;
	int ret_code;
	char* sec_new_passwd;
	
	/* Chequeo que ingrese la confirmacion de la clave correctamente */
	if( strcmp(new_passwd, rep_new_passwd) != 0 )
		return NEW_PASSWD_INVALID;
	/* Encripto la nueva clave */
	if( (sec_new_passwd=malloc(strlen(new_passwd)+1)) == NULL )
		return CHANGE_ERROR;
	des_encipher(new_passwd,sec_new_passwd,log_passwd);
	/* Paquete de pedido */
	strcpy(new_client_info.passwd,sec_new_passwd);
	strcpy(new_client_info.user,log_user);
	/* Mando el pedido */
	ret_code = SendRequest(	__NEW_PASSWD__, 1, &new_client_info, sizeof(login_t));	
	/* Proceso la respuesta */
	switch (ret_code) {
		case __CHANGE_OK__:
			ret = CHANGE_OK;
			/* Guardo la nueva contreaseña */
			strcpy(log_passwd, new_passwd);
			break;
		case __USER_IS_NOT_LOG__:
			ret = CHANGE_LOG_ERROR;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_ACCESS_DENY__:
			ret = CHANGE_ACCESS_DENY;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case CONNECT_ERROR:
			ret = CHANGE_ERROR;
			break;	
		default:
			ret = CHANGE_CONNECT_ERROR;
			break;
	}
	return ret;
}

client_user_reg
UserRegistration(char *user, char *passwd, char *rep_passwd, char *mail, char *desc, int level)
{
	client_user_reg ret = REG_OK;
	client_t new_user;
	int ret_code;

	/* Controlo que las clave de seguridad sea igual que la clave ingresada */
	if( strcmp(passwd, rep_passwd) != 0 ) {
		return REG_PASSWD_ERROR;
	}
	
	/* Armo el pedido */
	strcpy(new_user.user,user);
	strcpy(new_user.passwd,passwd);
	strcpy(new_user.mail,mail);
	strcpy(new_user.desc,desc);
	new_user.level = level;
		
	/* Mando el pedido */
	ret_code = SendRequest(__REG_USER__, 1, &new_user, sizeof(client_t));
	/* Proceso la respuesta */
	switch (ret_code) {
		case __REG_USER_ERROR__:
			ret = REG_USER_EXISTS;
			break;
		case __REG_ERROR__:
			ret = REG_ERROR;
			break;
		case __REG_OK__:
			ret = REG_OK;
			break;
		case CONNECT_ERROR:
			ret = REG_ERROR;
			break;
		default:
			ret = LOGIN_CONNECT_ERROR;
			break;
	}
	
	return ret;
}

client_logout_status
UserLogout(void)
{
	client_logout_status ret = LOG_OUT_OK;
	int ret_code;
			
	/* Mando el pedido */
	ret_code = SendRequest(__LOG_OUT__, 1, NULL, 0);
	/* Proceso la respuesta */
	switch (ret_code) {
		case __LOG_OUT_ERROR__:
			ret = LOG_OUT_ERROR;
			break;
		case __USER_ACCESS_DENY__:
			ret = LOG_OUT_ACCES_DENY;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_IS_NOT_LOG__:
			ret = LOG_OUT_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __LOG_OUT_OK__:
			strcpy(log_user, "anonimo");
			strcpy(log_passwd, "anonimo");
			ret = LOG_OUT_OK;
			break;
		default:
			ret = LOG_OUT_CONNECT_ERROR;
			break;
	}
	
	return ret;
}

client_download_status
UserDownload(char * ticket)
{
	client_download_status ret = DOWNLOAD_OK;
	request_t request;
	download_header_t download_info;
	FILE *fd;
	
	strcpy(request.ticket,ticket);
	/* Mando el pedido */
	download_info = SendDownloadRequest(&request, sizeof(request_t));
	/* Proceso la respuesta */
	switch (download_info.ret_code) {
		case __DOWNLOAD_ERROR__:
			ret = DOWNLOAD_ERROR;
			break;
		case __USER_ACCESS_DENY__:
			ret = DOWNLOAD_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_IS_NOT_LOG__:
			ret = DOWNLOAD_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __DOWNLOAD_START__:
			ret = DOWNLOAD_OK;
			/* Reservo espacio en el disco local para el archivo que voy a bajar */
			if( (fd = CreateFile(download_info.title,download_info.size)) == NULL )
				ret = DOWNLOAD_ERROR;
			/* Comienzo a descargar */
			else if( StartDownload(fd) == ERROR )
				ret = DOWNLOAD_ERROR;
			break;
		default:
			ret = DOWNLOAD_CONNECT_ERROR;
			break;
	}
	
	return ret;
}


/* Static Functions */
static unsigned long 
SendRequest(u_size op_code,u_size total_objects,void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	int ret;
	ack_t *ack_ptr;
	
	/* Tipo de pedido */
	header.opCode = op_code;
	header.total_objects = total_objects;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(sizeof(header_t)+size)) == NULL )
		return CONNECT_ERROR;	
	memmove(to_send, &header, sizeof(header_t));
	/* Chequeo si realmente se manda un paquete asociado al pedido */
	if( packet != NULL ) {
		memmove(to_send+sizeof(header_t), packet, size);
	}	
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return CONNECT_ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,sizeof(header_t)+size);
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_ptr = receiveTCP(socket);	
	ret = ack_ptr->ret_code;
	free(ack_ptr);
	/* Cierro la conexion????? */
	close(socket);
	return ret;
}

static download_header_t
SendDownloadRequest(void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	download_header_t download_info,*download_info_ptr;
	
	download_info.ret_code = CONNECT_ERROR;
	
	/* Tipo de pedido */
	header.opCode = __DOWNLOAD__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(sizeof(header_t)+size)) == NULL )
		return download_info;	
	memmove(to_send, &header, sizeof(header_t));
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return download_info;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,sizeof(header_t)+size);
	free(to_send);
	/* Espero por la respuesta del servidor */
	download_info_ptr = receiveTCP(socket);	
	download_info = *download_info_ptr;
	free(download_info_ptr);
	/* Cierro la conexion????? */
	close(socket);
	return download_info;
	
}

static status
SendSignal(u_size op_code, void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	
	/* Tipo de senial */
	header.opCode = op_code;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(sizeof(header_t)+size)) == NULL )
		return ERROR;	
	memmove(to_send, &header, sizeof(header_t));
	memmove(to_send + sizeof(header_t),packet,size);
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,sizeof(header_t)+size);
	free(to_send);
	
	close(socket);
	return OK;	
}

static status
ListenMovie(FILE *fd,char *port)
{
	int passive_s,ssock;
	unsigned long total_packets;
	unsigned long n_packet;
	boolean exit;
	download_t header;
	download_start_t start;
	void *packet;
	
	/* Preparo el puerto que va a escuchar la conexion */
	if( (passive_s=prepareTCP(HOST_DOWNLOAD,port,prepareServer)) < 0 ) {
		return FATAL_ERROR;
	}	
	if( (listenTCP(passive_s,10)) < 0 ) {
		return FATAL_ERROR;
	}
	
	/* Mando la senial al server pidiendo el inicio de la descarga */
	strcpy(start.port,port);
	strcpy(start.ip,HOST_DOWNLOAD);
	if( SendSignal(__DOWNLOAD_START_OK__, &start, sizeof(download_start_t)) == ERROR )
		return ERROR;
	
	exit = FALSE;
	n_packet = 0;
	ssock=acceptTCP(passive_s);
	while (!exit) {
		/* Recibo un paquete */
		packet = receiveTCP(ssock);
		memmove(&header, packet, sizeof(download_t));	
		/* Lo bajo a disco */
		PutFileData(fd,_FILE_SIZE_, header.n_packet,packet+sizeof(download_t),header.size);
		/* Verifico la cantidad total de paquetes a descargar */
		total_packets = header.total_packets;
		free(packet);
		n_packet++;
		/* Me fijo si llego a la cantidad total de paquetes bajados */
		if( n_packet >= total_packets )
			exit = TRUE;
	}
	closeTCP(ssock);
	closeTCP(passive_s);
	
	return OK;
}

static status 
StartDownload(FILE *fd)
{	
	/* Creo un proceso que se encargue de recibir los paquetes de descarga */
	switch( fork() ) {
		case 0:			
			/* Falta incrementar el puerto para atender multiples conexiones */
			if( (ListenMovie(fd,"1050")) != OK ) {
			   exit(EXIT_FAILURE);
			}
			fclose(fd);
			exit(EXIT_SUCCESS);
			break;
		case -1:
			/* fclose(fd); */
			return ERROR;
			break;
		default:
			return OK;
			break;
	}
}
