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
	
	client_t cliente;
	
	strcpy(cliente.user,"ehgato");
	strcpy(cliente.passwd,"hola");
	strcpy(cliente.mail,"marolio@heman.com");
	strcpy(cliente.desc,"pibe chorro");
	cliente.level=10;
	
	ClientAdd(ld, cliente);
	
	return OK;
}

void
EndServer(void)
{
	EndLdap(ld);
}