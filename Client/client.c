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

char log_user[MAX_USER_LEN];
char log_passwd[MAX_USER_PASS];

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

client_login_status
UserLogin(char *user, char* passwd)
{
	int size;
	ack_t *ack_ptr;
	int ret;
	void * to_send;
	header_t header;
	int socket;

	/* Tipo de pedido */
	header.opCode = __USER_LOGIN__;
	/* Cantidad de paquetes en el pedido */
	header.total_objects = 1;
	
	/* Paquete del pedido */
	login_t log;
	strcpy(log.user,user);
	strcpy(log.passwd,passwd);
	
	/* Me conecto al servidor */
	if( (socket=connectTCP("127.0.0.1","1044")) < 0 ){
		return LOGIN_CONNECT_ERROR;
	}
	
	size = sizeof(header_t) + sizeof(login_t);
	to_send = malloc(size);
	
	/* Armo el paquete completo a mandar */
	memmove(to_send,&header,sizeof(header_t));
	memmove(to_send+sizeof(header_t),&log,sizeof(login_t));
	/* Mando el paquete al servidor */
	sendTCP(socket,to_send,size);	
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_ptr = receiveTCP(socket);	
	/* Proceso la respuesta */
	switch (ack_ptr->ret_code) {
		case __LOGIN_OK__:
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
		case __USER_IS_LOG__:
			ret =  LOGIN_USER_IS_LOG;
			break;
		default:
			ret = LOGIN_CONNECT_ERROR;
			break;
	}
	free(ack_ptr);
	/* Cierro la conexion */
	close(socket);	
	
	return ret;
}

client_change_passwd_status
UserChangePasswd(char *new_passwd, char *rep_new_passwd)
{
	client_change_passwd_status ret;
	void *to_send;
	login_t new_client_info;
	header_t header;
	int socket;
	ack_t *ack_ptr;
	
	/* Chequeo que ingrese la confirmacion de la clave correctamente */
	if( strcmp(new_passwd, rep_new_passwd) != 0 )
		return NEW_PASSWD_INVALID;
	
	/* Tipo de pedido */
	header.opCode = __NEW_PASSWD__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);	
	/* Paquete de pedido */
	strcpy(new_client_info.passwd,new_passwd);
	strcpy(new_client_info.user,log_user);
	
	if( (to_send = malloc(sizeof(header_t)+sizeof(login_t))) == NULL )
		return CHANGE_ERROR;
	
	memmove(to_send, &header, sizeof(header_t));
	memmove(to_send+sizeof(header_t), &new_client_info, sizeof(login_t));
	
	/* Me conecto al servidor */
	if( (socket=connectTCP("127.0.0.1","1044")) < 0 ){
		free(to_send);
		return LOGIN_CONNECT_ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,sizeof(header_t)+sizeof(login_t));
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_ptr = receiveTCP(socket);	
	/* Proceso la respuesta */
	switch (ack_ptr->ret_code) {
		case __CHANGE_OK__:
			ret = CHANGE_OK;
			strcpy(log_passwd, new_passwd);
			break;
		case __USER_IS_NOT_LOG__:
			ret = CHANGE_LOG_ERROR;
			break;
		case __USER_ACCESS_DENY__:
			ret = CHANGE_ACCESS_DENY;
			break;		
		default:
			ret = CHANGE_CONNECT_ERROR;
			break;
	}
	free(ack_ptr);
	/* Cierro la conexion???? */
	close(socket);		
	
	return ret;
}	
