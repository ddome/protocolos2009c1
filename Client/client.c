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

/* Static Functions */

static unsigned long SendRequest(u_size op_code,u_size total_objects,void *packet, u_size size); 

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
	
	/* Chequeo que ingrese la confirmacion de la clave correctamente */
	if( strcmp(new_passwd, rep_new_passwd) != 0 )
		return NEW_PASSWD_INVALID;

	/* Paquete de pedido */
	strcpy(new_client_info.passwd,new_passwd);
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
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_ACCESS_DENY__:
			ret = CHANGE_ACCESS_DENY;
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
	client_user_reg ret = REG_OK;
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
			break;
		case __USER_IS_NOT_LOG__:
			ret = LOG_OUT_USER_NOT_LOG;
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
	if( (socket=connectTCP("127.0.0.1","1044")) < 0 ){
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
