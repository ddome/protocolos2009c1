#include <stdio.h>
#include "../Common/defines.h"
#include "../Common/app.h"
#include "../Common/TCPLib.h"


int main (int argc, const char * argv[]) {
	login_t log;
	int ret;
	int aux;
	int size;
	ack_t * resp;
	void * toSend;
	header_t header;
	strcpy(log.user,"nbombau");
	strcpy(log.passwd,"secret");
	
	ret=connectTCP("127.0.0.1","1234");
	if(ret<0)
	{
		printf("Se produjo un error al conectar.\n");
		return -1;
	}
	size=sizeof(header_t) + sizeof(login_t);
	toSend=malloc(size);
	memmove(toSend,&header,sizeof(header_t));
	memmove(toSend+sizeof(header_t),&log,sizeof(login_t));
	aux=sendTCP(ret,toSend,size);
	
	
	resp=receiveTCP(ret,sizeof(ack_t));
	printf("El ACK fue de: %ld\n",resp->ret_code);
	close(ret);
	return 0;
}
