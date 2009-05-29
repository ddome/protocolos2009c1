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
#include "../Common/UDPLib.h"
#include "../Common/des/include/encrypt.h"

/* Lista con los servers disponibles */
LIST payment_servers;

/* GetData(pack) */
u_size GetAckData(payment_server_t pack,void **data_ptr);

status 
InitLookup(void)
{
	payment_servers = list_load("serverlist", MAX_SERVER_LEN, sizeof(payment_server_t));
	return OK;
}	

status 
StartLookup(void)
{	
	int socket;
	void *data;
	host_t dest;
	socket = prepareUDP("127.0.0.1", "1060");
	
	while(1) {
		
		data = receiveUDP(socket,&dest);
		if( Session(data,socket,dest) == FATAL_ERROR )
			exit(EXIT_FAILURE);	
	}	
	
	return OK;
}

void
EndLookup(void)
{
	free(payment_servers);
}


/* Atencion de pedidos */

status
Session(void *data,int socket,host_t dest)
{
	server_request_t req;
	payment_server_t *ack;
	void *ack_data;
	u_size ack_size;
	
	/* GetPack(data) */
	memmove(req.name,data,MAX_SERVER_LEN);
	
	ack = GetServer(req.name);
	ack_size = GetAckData(*ack,ack_data);
	
	sendUDP(socket,ack_data,dest);
	return OK;
}

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

/* GetData(pack) */

u_size
GetAckData(payment_server_t pack,void **data_ptr)
{
	u_size pos;
	void *data;

	if( (data=malloc(MAX_SERVER_LEN+MAX_HOST_LEN
					 +MAX_PORT_LEN+MAX_SERVER_KEY)) == NULL )
		return -1;
	
	pos = 0;
	memmove(data, pack.name, MAX_SERVER_LEN);
	pos += MAX_SERVER_LEN;
	memmove(data, pack.host, MAX_HOST_LEN);
	pos += MAX_HOST_LEN;
	memmove(data, pack.port, MAX_PORT_LEN);
	pos += MAX_PORT_LEN;
	memmove(data, pack.key, MAX_SERVER_KEY);
	pos += MAX_SERVER_KEY;
		
	return pos;
}

