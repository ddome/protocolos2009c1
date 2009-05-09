/*
 *  server.c
 *  MovieStoreServer
 */

#include <stdio.h>

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
	login_t log;	
	ack_t to_ack;
	
	while(1) {
		
		if( (newSocket=acceptTCP(passive_s)) <= 0 ) {
			printf("Fallo acceptTCP() - retCode=(%d)\n",newSocket);
			return FATAL_ERROR;
		}

		paquete=receiveTCP(newSocket, sizeof(header_t)+sizeof(login_t));
		memmove(&log,paquete+sizeof(header_t),sizeof(login_t));
		//free(paquete);
		printf("Username: (%s) - Password: (%s)\n",log.user,log.passwd);
		to_ack.ret_code = 1;
		sendTCP(newSocket, &to_ack, sizeof(ack_t));
		
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
