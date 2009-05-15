/*
 *  server.c
 *  MovieStoreServer
 */

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

#include "../Common/genlib.h"
#include "client_ldap.h"
#include "server.h"
#include "../Common/TCPLib.h"
#include "hashADT.h"
#include "../Common/des/include/encrypt.h"

/* Variable global que guarda la coneccion con el servidor LDAP */
LDAP *ld;

/* Puerto de conexion del servidor */
int passive_s;

/* Informacion de los usuarios online */
hashADT users_online;

/* Static functions */

static boolean UserCanAcces(char *user,char *passwd);

static status UserDelete(char *user,char *passwd);


status 
InitServer(void)
{
	int ret;
	
	/* Iniciar el servidor ldap */
	
	if( (ld=InitLdap()) == NULL ) {
		return FATAL_ERROR;
	}
		
	/* Iniciar TCP */
	
	if( (passive_s=prepareTCP("127.0.0.1","1044",prepareServer)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",passive_s);
		return FATAL_ERROR;
	}	
	if( (ret=listenTCP(passive_s,10)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",ret);
		return FATAL_ERROR;
	}
		
	return OK;
}	
	

status 
StartServer(void)
{
	int ssock;
	void * packet;
	fd_set rfds;
	fd_set afds;	
	int fd, nfds;
	
	/* Mantengo la informacion de los usuarios online */
	users_online = NewHash(150, UsersComp, UsersHash);
	
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(passive_s,&afds);
	
	while(1) {
		
		memcpy(&rfds, &afds, sizeof(rfds));
		
		if( select(nfds, &rfds, NULL, NULL,NULL) < 0 ) {
			fprintf(stderr, "select: %s\n", strerror(errno));
			return FATAL_ERROR;
		}
		
		/* Si es una nueva conexion la agrego */
		if (FD_ISSET(passive_s, &rfds)) {
						
			if( (ssock=acceptTCP(passive_s)) <= 0 ) {
				printf("Fallo acceptTCP() - retCode=(%d)\n",ssock);
				return FATAL_ERROR;
			}
			
			FD_SET(ssock, &afds);
		}
		
		/* Atiendo cada pedido */
		for(fd=0; fd<nfds; ++fd) {
			if (fd != passive_s && FD_ISSET(fd, &rfds)) {
								
				packet=receiveTCP(fd);
				/* Proceso el packet */
				if( Session(packet,fd) == OK ) {
					close(fd); // Tengo que cerrar la conexion?
					FD_CLR(fd, &afds);
				}
				else {
					return FATAL_ERROR;
				}
					
			}
		}
	}	
	closeTCP(passive_s);
	
	return OK;
}

void
EndServer(void)
{
	EndLdap(ld);
}


/* Atencion de pedidos */


status
Session(void *packet,int socket)
{
	header_t header;	
	login_t log;
	client_t client;
	
	
	/* Levanto el header del paquete */
	memmove(&header, packet, sizeof(header));
	
	switch (header.opCode) {
		case __USER_LOGIN__:
			/* Logueo al ususario */
			memmove(&log, packet + sizeof(header_t), sizeof(login_t) );	
			fprintf(stderr,"Llego un pedido de --login-- de user:%s passwd:%s\n",log.user,log.passwd);
			return UserLogin(log,socket);
			break;
		case __NEW_PASSWD__:
			/* Cambio de clave */
			memmove(&log, packet + sizeof(header_t), sizeof(login_t) );
			fprintf(stderr,"Llego un pedido de --password-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserNewPasswd(log,socket,header.user,header.passwd);
			break;
		case __REG_USER__:
			/* Registro de un nuevo usuario */
			memmove(&client, packet + sizeof(header_t), sizeof(client_t) );
			fprintf(stderr,"Llego un pedido de --new-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserRegister(client,socket);
			break;
		case __LOG_OUT__:
			/* Desconectar usuario */
			fprintf(stderr,"Llego un pedido de --logout-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserLogout(socket, header.user, header.passwd);
			break;		
		default:
			fprintf(stderr, "No se reconocio el op_code:%d\n",header.opCode);
			return FATAL_ERROR;
			break;
	}
	
}

status
UserLogin(login_t log,int socket)
{
	int ret;	
	ack_t to_ack;
	char *user;
	char *passwd;
	login_t *log_ptr;
			
	user = CopyString(log.user);
	passwd = CopyString(log.passwd);
		
	ret = __LOGIN_OK__;
		
	if( !UserExist(ld, user) ) 
		ret = __USER_ERROR__;	
	else if( !PasswdIsValid(ld, user, passwd) )
		ret = __PASSWD_ERROR__;
	
	if( ret == __LOGIN_OK__ ) {
		/* Si ya esta logueado no lo vuelve a insertar */
		if(  Lookup(users_online, &log) == -1 ) {
			if( (log_ptr = malloc(sizeof(login_t))) == NULL )
				return FATAL_ERROR;
			*log_ptr = log;
			if( HInsert(users_online, log_ptr) == 0 )
				return FATAL_ERROR;
		}
		else
			ret = __USER_IS_LOG__;
	}
	/* Mando al respuesta */
	to_ack.ret_code = ret;
	sendTCP(socket, &to_ack, sizeof(ack_t));
	
	return OK;
}

status 
UserNewPasswd(login_t log,int socket, char *user,char *passwd)
{
	char *aux_user;
	char *aux_passwd;
	int ret = __CHANGE_OK__;
	ack_t ack;
	login_t *log_ptr;
	char *des_passwd;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	
	/* Si pasa los controles, cambio su clave */
	if( ret == __CHANGE_OK__ ) {		
		/* Desencripto la nueva clave */
		des_passwd = malloc(strlen(log.passwd)+1);
		des_decipher(log.passwd, des_passwd, passwd);
		fprintf(stderr, "--%s--\n",des_passwd);
		strcpy(log.passwd,des_passwd);
		aux_user = CopyString(log.user);
		aux_passwd = CopyString(log.passwd);
		/* Llamado a la funcion que cambia la password en el servidor ldap */
		ChangePasswd(ld, aux_user, aux_passwd);
		free(aux_user);
		free(aux_passwd);
		free(des_passwd);
		/* Borro e inserto el usuario con la nueva password en login_users */
		UserDelete(user, passwd);
		fprintf(stderr, "--%s--\n",log.passwd);
		if( (log_ptr = malloc(sizeof(login_t))) == NULL )
			return FATAL_ERROR;
		*log_ptr = log;
		if( HInsert(users_online, log_ptr) == 0 )
			return FATAL_ERROR;		
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));
		
	return OK;
}

status
UserRegister(client_t client,int socket)
{
	int ret = __REG_OK__;
	ack_t ack;
		
	/* Llamo a la funcion que pregunta si un nombre de usuario ya existe en la base ldap */
	if( UserExist(ld, client.user) )
		ret = __REG_USER_ERROR__;

	if( ret == __REG_OK__ ) {		
		/* Llamo a la funcion que agrega un cliente a la base ldap */
		if( ClientAdd(ld,client) == ERROR )
			ret = __REG_ERROR__;
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));
	
	return OK;
}

status
UserLogout(int socket, char *user, char *passwd)
{
	int ret = __LOG_OUT_OK__;
	ack_t ack;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	/* Borro al usuario de la lista de usuarios conectados */
	if( ret == __LOG_OUT_OK__ ) { 
		if( UserDelete(user,passwd) == ERROR )
			ret = __LOG_OUT_ERROR__;
	}	
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));	
	
	return OK;
}


/* Funciones del manejo de la tabla de hashing */

int
UsersComp( void *v1, void *v2 )
{
	login_t *c1=(login_t *)v1;
	login_t *c2=(login_t *)v2;
	return strcmp( c1->user, c2->user );
}

int
UsersHash( void *v1, int size )
{
	int hash = 0;
	login_t *c1 = (login_t *)v1;
	int len  = strlen(c1->user);
	int i;
	
	for(i=0;i<len;i++){
		hash += c1->user[i];
	}
	
	return hash % size;
}

/* Static Functions */

/* Funcion que controla la identificacion de los usuarios */
static boolean 
UserCanAcces(char *user,char *passwd)
{
	int pos;
	login_t id;
	login_t *aut;
	strcpy(id.user, user);
	strcpy(id.passwd, passwd);
	
	/* Control de seguridad del usuario */
	if( (pos=Lookup(users_online,&id)) == -1 ) {
		return FALSE;
	}	
	/* Control de seguridad del password */	
	aut = GetHElement(users_online,pos);
	if( strcmp(aut->passwd,passwd ) != 0 ) {
		return FALSE;
	}	
	
	return TRUE;
}

static status
UserDelete(char *user,char *passwd)
{
	int pos;
	login_t id;
	login_t *aut;
	strcpy(id.user, user);
	strcpy(id.passwd, passwd);
	/* Busco el elemento previamente para poder liberarlo */
	if( (pos=Lookup(users_online,&id)) == -1 ) {
		return ERROR;
	}	
	aut = GetHElement(users_online,pos);
	/* Borro el elemento de la lista de usuarios online */
	HDelete(users_online, &id);
	/* Libero el puntero usado */
	free(aut);
	return OK;
}



