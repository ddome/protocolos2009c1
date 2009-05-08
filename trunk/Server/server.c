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

status 
InitServer(void)
{	
	if( (ld=InitLdap()) == NULL ) {
		return FATAL_ERROR;
	}
		
	return OK;
}	
	

status 
StartServer(void)
{
	int socket;
	int newSocket;
	int ret;
	void * paquete;
	login_t log;
	/* Inicializar socket de escucha */
	/* Iniciar comunicacion */
	/* Interpretar mensaje */
	/* Crear proceso de ser necesario y atender proceso */
	
	/*client_t bombau;	
	strcpy(bombau.user,"nbombau");
	strcpy(bombau.passwd,"secret");
	strcpy(bombau.mail,"bombax@gmail.com");
	strcpy(bombau.desc, "sarasa");
	bombau.level = 10;
	ClientAdd(ld, bombau);
	 */
	
	ack_t toAck;
	toAck.ret_code=1;
	printf("Puto\n");
	socket=prepareTCP("127.0.0.1","1044",prepareServer);
	if(socket<=0)
	    {
		printf("Fallo prepareTCP() - retCode=(%d)\n",socket);
		return -1;
	    }
	ret=listenTCP(socket,10);
	if(ret<0)
	    {
		printf("Fallo listenTCP() - retCode=(%d)\n",ret);
		return -1;
	    }
	while(1)
	{
		printf("Llegue\n");
		newSocket=acceptTCP(socket);
		printf("Pase\n");
		if(newSocket<=0)
		    {
			printf("Fallo acceptTCP() - retCode=(%d)\n",newSocket);
			return -1;
		    }

		paquete=receiveTCP(newSocket, sizeof(header_t)+sizeof(login_t));
		memmove(&log,paquete+sizeof(header_t),sizeof(login_t));
		//free(paquete);
		printf("Username: (%s) - Password: (%s)\n",log.user,log.passwd);
		
		sendTCP(newSocket, &toAck, sizeof(ack_t));
		
		closeTCP(newSocket);
	}	
	closeTCP(socket);
	
	return OK;
}

void
EndServer(void)
{
	EndLdap(ld);
}
