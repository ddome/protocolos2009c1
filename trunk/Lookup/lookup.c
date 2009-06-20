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
#include "hashADT.h"
#include "../Common/app.h"
#include "../Common/genlib.h"
#include "../Common/des/include/encrypt.h"
#include "lookup.h"

/* Lista con los servers disponibles */
hashADT payment_servers;

static payment_server_t * GetServer(char *server_name);

/* GetData(pack) */
u_size GetAckData(payment_server_t pack,void **data_ptr);

int
ServersComp( void * v1, void *v2 )
{
	payment_server_t *t1,*t2;
	
	t1 = v1;
	t2 = v2;
	
	return strcmp(t1->name, t2->name);
}

int
ServerHash( void *v, int size )
{
	int i;
	payment_server_t *t = v;
	int num = 0;
	
	i=0;
	while (t->name[i] != '\0' ) {
		num += (t->name)[i];
		i++;
	}	
	return  num % size;
}

int
ServerSave(FILE *fd,void *data)
{	
	payment_server_t *f =data;
	fprintf(fd,"%s;%s;%s;%s\n",f->name,f->host,f->port,f->key);
	
	return 0;
}

void *
ServerLoad(FILE *fd)
{	
	payment_server_t *f = malloc(sizeof(payment_server_t));
	char line[500];
	char *token;
	
	if( fgets(line,500,fd) == NULL )
		return NULL;
	
	token = strtok (line,";");
	if( token == NULL )
		return NULL;
	strcpy(f->name,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->host,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->port,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->key,token);
	
	return (void*)f;
}



status 
InitLookup(void)
{
	payment_servers = LoadHashTable(LOCATION, sizeof(payment_server_t), ServersComp, ServerHash, ServerSave, ServerLoad);
	
	return OK;
}	

status 
StartLookup(void)
{	
	int socket;
	void *data;
	host_t dest;
	socket = prepareUDP("127.0.0.1", "1070");
	
	while(1) {
		data = receiveUDP(socket,&dest);
		sleep(500);
		fprintf(stderr, "LLEGOOOO");
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
	
	printf("\n%s\n",req.name);
	
	if( (ack = GetServer(req.name)) == NULL ) {
		ack = malloc(sizeof(payment_server_t));
		strcpy(ack->name,"NOT_EXISTS");
	}
	
	fprintf(stderr,"%s %s %s %s\n",ack->name,ack->host,ack->port,ack->key);
	
	ack_size = GetAckData(*ack,&ack_data);
	
	fprintf(stderr,"%ld\n",ack_size);
	
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
	if( HInsert(payment_servers, aux) == -1 )
		return ERROR;
	else
		return OK;	
}

static payment_server_t *
GetServer(char *server_name)
{
	payment_server_t aux;
	int pos;
	
	strcpy(aux.name, server_name);
	if( (pos=Lookup(payment_servers, &aux)) == -1 )
		return NULL;
		
	return GetHElement(payment_servers, pos);
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
	memmove(data+pos, pack.name, MAX_SERVER_LEN);
	pos += MAX_SERVER_LEN;
	memmove(data+pos, pack.host, MAX_HOST_LEN);
	pos += MAX_HOST_LEN;
	memmove(data+pos, pack.port, MAX_PORT_LEN);
	pos += MAX_PORT_LEN;
	memmove(data+pos, pack.key, MAX_SERVER_KEY);
	pos += MAX_SERVER_KEY;
	
	*data_ptr = data;
		
	return pos;
}

