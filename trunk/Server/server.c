/*
 *  server.c
 *  MovieStoreServer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Common/genlib.h"
#include "client_ldap.h"
#include "server.h"
#include "../Common/TCPLib.h"


/* Variable global que guarda la coneccion con el servidor LDAP */
LDAP *ld;

/* Puerto de conexion del servidor */
int passive_s;

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
	int newSocket;
	void * paquete;
	
	while(1) {
		
		if( (newSocket=acceptTCP(passive_s)) <= 0 ) {
			printf("Fallo acceptTCP() - retCode=(%d)\n",newSocket);
			return FATAL_ERROR;
		}

		paquete=receiveTCP(newSocket, sizeof(header_t)+sizeof(login_t));
		//memmove(&log,paquete+sizeof(header_t),sizeof(login_t));
		Session(paquete,newSocket);
	
		closeTCP(newSocket);
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
	void *info;
	
	memmove(&header, packet, sizeof(header));
	
	switch (header.opCode) {
		case __USER_LOGIN__:
			if( (info=malloc(sizeof(login_t))) == NULL ) {
				return FATAL_ERROR;
			}
			memmove(info, packet + sizeof(header_t), sizeof(login_t) );
			return UserLogin(*(login_t*)info,socket);
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
	
	void *prueba=NULL;
	
	prueba = malloc(10);
	
	user=malloc(strlen(log.user)+1);	
	passwd=malloc(strlen(log.passwd)+1);
	
	/*if( (user=malloc(strlen(log.user)+1)) == NULL )
		return FATAL_ERROR;
	if( (passwd=malloc(strlen(log.passwd)+1)) == NULL )
		return FATAL_ERROR;
	*/
	
	strcpy(user,log.user);
	strcpy(passwd,log.passwd);
		
	ret = __LOGIN_OK__;
		
	if( !UserExist(ld, user) )
		return __USER_ERROR__;
	if( !PasswdIsValid(ld, user, passwd) )
		return __PASSWD_ERROR__;
		
	/* Debugueo */
	printf("Username: (%s) - Password: (%s)\n",log.user,log.passwd);
	
	to_ack.ret_code = ret;
	sendTCP(socket, &to_ack, sizeof(ack_t));
	
	return OK;
}



