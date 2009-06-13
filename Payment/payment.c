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
#include "../Common/TCPLib.h"
#include "../Common/des/include/encrypt.h"
#include "../Common/fileHandler.h"

hashADT client_data_base;

int passive_s;

status 
InitPaymentServer(void)
{
	int ret;
		
	/* Iniciar TCP */
	if( (passive_s=prepareTCP(HOST_PAYMENT,PORT_PAYMENT,prepareServer)) < 0 ) {
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
StartPaymentServer(void)
{
	int ssock;
	void * data;
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
								
				data=receiveTCP(fd);
				/* Proceso el paquete */
				if( Session(data,fd) != FATAL_ERROR ) {
					close(fd); // Tengo que cerrar la conexion?
					FD_CLR(fd, &afds);
					free(data);
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
EndPaymentServer(void)
{

}

/*******************************************************************************************************/
/*                                       Atencion de pedidos                                           */
/*******************************************************************************************************/

status
Session(void *data,int socket)
{
	
		
}

