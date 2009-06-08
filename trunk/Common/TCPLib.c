#include "TCPLib.h"

int prepareTCP(const char * host,const char * port,type_t type)
{
	int socketFD=0;
	struct sockaddr_in sAddress;
	//struct hostent	*phe;
	struct protoent *ppe;
	
	/*if ( (ppe = getprotobyname("tcp")) == 0)
	{
	    //Agregar syslog
	    socketFD=GEN_ERROR;
	}*/
	
	if( (socketFD<0) || (socketFD=socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		//Agregar el syslog
		socketFD=GEN_ERROR;
	}
	else
	{
		memset(&sAddress, 0, sizeof(sAddress));
		sAddress.sin_family=AF_INET;
		sAddress.sin_port=htons((unsigned short)atoi(port));
		//if(type==prepareServer)
			sAddress.sin_addr.s_addr = INADDR_ANY;
		/*else
		{
			if ( (phe = gethostbyname(host)) )
				memcpy(&sAddress.sin_addr, phe->h_addr, phe->h_length);
			else if ((sAddress.sin_addr.s_addr = inet_addr(host))==INADDR_NONE)
				socketFD=INV_HOST;
		}*/
		if(socket>=0)
		{
			if( bind(socketFD,(struct sockaddr *)&sAddress,
						sizeof(sAddress)) == -1 )
			{
			    switch(errno)
			    {
				case EACCES: printf("The address is protected, and the user is not the superuser.\n");
					    break;
				case EADDRINUSE: printf("The given address is already in use.\n");
					    break;
				case EBADF: printf("sockfd is not a valid descriptor.\n");
					    break;
				case EINVAL: printf("The socket is already bound to an address.\n");
					    break;
				case ENOTSOCK: printf("sockfd is a descriptor for a file, not a socket.\n");
					    break;
				default: printf("Otro error\n");
			    }
				
			    /*printf("1\n");
				//Agregar syslog
				if(errno==EADDRINUSE)
				{
					*ver si esto esta bien, la idea es que avise cuando un
					 * puerto ya esta ocupado.
					socketFD=ADD_IN_USE;
				}
				else
				{
					socketFD=GEN_ERROR;
				}*/
			}
		}
	}
	
	return socketFD;	
}

int listenTCP(int socketFD,int max_queue_len)
{
	int ret=0;
	/*The  max_queue_len  argument  defines  the maximum length to which the
	 * queue of pending connections for the socket may grow.  If a connection
	 * request arrives when the queue is full, the client may receive an error
	 * with an indication of ECONNREFUSED or, if the underlying protocol
	 * supports retransmission, the request may be ignored so that a later
	 * reattempt at connection succeeds.*/
 	if((ret=listen(socketFD,max_queue_len))==-1)
 	{
 		//Agregar syslog
 		ret=GEN_ERROR;
 	}
	return ret;
}

int connectTCP(const char * host,const char * port)
{
	int socketFD=0;
	struct sockaddr_in cAddress;
	struct hostent	*phe;
	if( (socketFD=socket(AF_INET,SOCK_STREAM,0)) == -1 )
	{
		//Agregar el syslog
		socketFD=GEN_ERROR;
	}
	else
	{
		memset(&cAddress, 0, sizeof(cAddress));
		cAddress.sin_family=AF_INET;
		cAddress.sin_port=htons((unsigned short)atoi(port));

		if ( (phe = gethostbyname(host)) )
			memcpy(&cAddress.sin_addr, phe->h_addr, phe->h_length);
		else if ((cAddress.sin_addr.s_addr = inet_addr(host))==INADDR_NONE)
			socketFD=INV_HOST;
			
		if(socketFD>=0)
		{
			if( connect(socketFD,(struct sockaddr *)&cAddress,sizeof(cAddress)) == -1 )
			{
				//Agregar syslog y switch de errores.
				socketFD=GEN_ERROR;
			}
		}
	}
	
	return socketFD;
}

int acceptTCP(int socketFD)
{
	int newSocketFD=0;
	//Los argumentos en NULL de accept son para informacion del cliente.
	newSocketFD=accept(socketFD,NULL,NULL);
	if(newSocketFD==-1)
	{
		//Agregar syslog
		switch(errno)
		{
			case ECONNABORTED:	newSocketFD=CONN_ABORTED;
								break;
			case EPERM:			newSocketFD=FIREWALL_PERM;
								break;
			default:			newSocketFD=GEN_ERROR;
		}
	}
	
	return newSocketFD;
}
/*
int sendTCP(int socketFD,void * data,size_t size)
{
	int ret=0;
	u_size header_size;
	
	header_size = size;
	
	// Mando el tamanio del paquete a levantar
	send(socketFD,&header_size,sizeof(u_size),0);
	
	if(send(socketFD,data,size,0)==-1);
	{
		//Agregar syslog
		switch(errno)
		{
			case ECONNRESET: 	ret=CONN_RST_PEER;
								break;
			case EINVAL:
			case EFAULT: 		ret=INV_ARGS;
								break;
			case EMSGSIZE: 		ret=MSG_SIZE_ERR;
								break;
			default:            ret=GEN_ERROR;
		}
	}
	return ret;
}*/


int sendTCP(int socketFD,void * data,size_t size)
{
	int ret=0;
	void * toSend;
	
	if( (toSend=malloc(sizeof(u_size) + size))==NULL )
	{
	    //Agregar syslog
	    return GEN_ERROR;
	}
	
	memmove(toSend,&size,sizeof(u_size));
	memmove(toSend+sizeof(u_size),data,size);

	if(send(socketFD,toSend,sizeof(u_size)+size,0)==-1);
	{
		//perror("Send: ");
		//Agregar syslog
		switch(errno)
		{
			case ECONNRESET: 	ret=CONN_RST_PEER;
								break;
			case EINVAL:
			case EFAULT: 		ret=INV_ARGS;
								break;
			case EMSGSIZE: 		ret=MSG_SIZE_ERR;
								break;
			default:            ret=GEN_ERROR;
		}
	}
	
	free(toSend);

	return ret;
}



void * receiveTCP(int socketFD)
{
	void * ret;
	u_size *header_size;
	
	if( (header_size = malloc(sizeof(u_size))) == NULL )
		return NULL;
	
	if(recv(socketFD,header_size,sizeof(u_size),MSG_WAITALL)<0)
	    return NULL;
	
	if( (ret=calloc(1,*header_size)) == NULL )
		return NULL;
	
	if(recv(socketFD,ret,*header_size,MSG_WAITALL)<0)
	    return NULL;
	
	free(header_size);
	
	return ret;
}

void closeTCP(int socketFD)
{
	//ver si combiene close o shutdown
	close(socketFD);
	return;
}

