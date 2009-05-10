/*
 *  client.c
 *  MovieStoreClient
 *
 *  Created by Damian Dome on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"
#include "Prompt.h"

status 
InitClient(void)
{
	return OK;
}

status
StartClient(void)
{	
	Prompt();
	return OK;
}

void
EndClient(void)
{
	return;
}

client_login_status
UserLogin(char *user, char* passwd)
{
	login_t log;
	int size;
	ack_t *ack_ptr;
	int ret;
	void * to_send;
	header_t header;
	int socket;
	
	strcpy(log.user,user);
	strcpy(log.passwd,passwd);
	
	header.opCode = __USER_LOGIN__;
	header.total_objects = 1;
	
	if( (socket=connectTCP("127.0.0.1","1044")) < 0 ){

		return LOGIN_CONNECT_ERROR;
	}
	
	size = sizeof(header_t) + sizeof(login_t);
	to_send = malloc(size);
	
	memmove(to_send,&header,sizeof(header_t));
	memmove(to_send+sizeof(header_t),&log,sizeof(login_t));

	sendTCP(socket,to_send,size);

	ack_ptr = receiveTCP(socket,sizeof(ack_t));	
	ret = ack_ptr->ret_code;
	
	//free(ack_ptr);
	close(socket);	
	
	return ret;
}


