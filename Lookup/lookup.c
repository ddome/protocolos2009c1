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

#include "list.h"
#include "../Common/app.h"
#include "../Common/genlib.h"
#include "../Common/TCPLib.h"
#include "../Common/des/include/encrypt.h"

/* Lista con los servers disponibles */
LIST payment_servers;

status 
InitLookup(void)
{
	payment_servers = list_load("serverlist", MAX_SERVER_LEN, sizeof(payment_server_t));
	return OK;
}	

status 
StartLookup(void)
{	
	while(1) {
		
	}	
	
	return OK;
}

void
EndLookup(void)
{
	free(payment_servers);
}


/* Atencion de pedidos */

/* Static Fuctions */

static status 
NewServer(payment_server_t server)
{
	payment_server_t *aux;
	
	if( (aux=malloc(sizeof(payment_server_t))) == NULL )
		return ERROR;
	*aux = server;
	if( list_add(payment_servers, aux->name, aux) == 1 )
		return OK;
	else
		return ERROR;	
}

static payment_server_t *
GetServer(char *server_name)
{
	return list_get(payment_servers, server_name);
}
