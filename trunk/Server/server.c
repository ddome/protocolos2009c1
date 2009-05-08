/*
 *  server.c
 *  MovieStoreServer
 */

#include <stdio.h>

#include "../Common/genlib.h"
#include "client_ldap.h"
#include "server.h"


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
	/* Inicializar socket de escucha */
	/* Iniciar comunicacion */
	/* Interpretar mensaje */
	/* Crear proceso de ser necesario y atender proceso */
		
	return OK;
}

void
EndServer(void)
{
	EndLdap(ld);
}