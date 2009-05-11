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
	int ssock;
	void * packet;
	fd_set rfds;
	fd_set afds;
	
	int fd, nfds;
	
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(passive_s,&afds);
		
	while(1) {
		
		memcpy(&rfds, &afds, sizeof(rfds));
		
		if( select(nfds, &rfds, NULL, NULL,NULL) < 0 ) {
			fprintf(stderr, "select: %s\n", strerror(errno));
			return FATAL_ERROR;
		}
		
		if (FD_ISSET(passive_s, &rfds)) {
						
			if( (ssock=acceptTCP(passive_s)) <= 0 ) {
				printf("Fallo acceptTCP() - retCode=(%d)\n",ssock);
				return FATAL_ERROR;
			}
			
			FD_SET(ssock, &afds);
		}
		
		for(fd=0;fd<nfds; ++fd) {
			if (fd != passive_s && FD_ISSET(fd, &rfds)) {
				packet=receiveTCP(fd, sizeof(header_t)+sizeof(login_t));				
				if( Session(packet,fd) == OK ) {
					close(fd);
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
	
	memmove(&header, packet, sizeof(header));
	
	switch (header.opCode) {
		case __USER_LOGIN__:			
			memmove(&log, packet + sizeof(header_t), sizeof(login_t) );				
			return UserLogin(log,socket);
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
			
	user = CopyString(log.user);
	passwd = CopyString(log.passwd);
		
	ret = __LOGIN_OK__;
		
	if( !UserExist(ld, user) ) 
		ret = __USER_ERROR__;	
	else if( !PasswdIsValid(ld, user, passwd) )
		ret = __PASSWD_ERROR__;
		
	/* Debugueo */
	printf("Username: (%s) - Password: (%s)\n",log.user,log.passwd);
	
	to_ack.ret_code = ret;
	sendTCP(socket, &to_ack, sizeof(ack_t));
	
	return OK;
}



