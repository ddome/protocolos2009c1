#include "UDPLib.h"

int prepareUDP(const char * host,const char * port)
{
	int socketFD=0;
	struct sockaddr_in sAddress;

	if( (socketFD<0) || (socketFD=socket(AF_INET,SOCK_DGRAM,0)) == -1 )
	{
		//Agregar el syslog
		socketFD=GEN_ERROR;
	}
	else
	{
		memset(&sAddress, 0, sizeof(sAddress));
		sAddress.sin_family=AF_INET;
		sAddress.sin_port=htons((unsigned short)atoi(port));
		sAddress.sin_addr.s_addr = INADDR_ANY;

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

int
sendUDP(int socketFD,void * data,size_t size,const char * host,const char * port)
{
    struct sockaddr_in sendAddress;
    struct hostent	*phe;
    void * toSend;
    
    memset(&sendAddress, 0x0, sizeof(sendAddress));
    sendAddress.sin_family=AF_INET;
    sendAddress.sin_port=htons((unsigned short)atoi(port));

    if ( (phe = gethostbyname(host)) )
	    memcpy(&sendAddress.sin_addr, phe->h_addr, phe->h_length);
    else if ((sendAddress.sin_addr.s_addr = inet_addr(host))==INADDR_NONE)
	    socketFD=INV_HOST;
    
    if(socketFD>=0)
    {
	if( (toSend=malloc(sizeof(u_size) + size))==NULL )
	{
	    //Agregar syslog
	    return GEN_ERROR;
	}
	
	memmove(toSend,&size,sizeof(u_size));
	memmove(toSend+sizeof(u_size),data,size);

	if(sendto(socketFD,toSend,sizeof(u_size) + size,0,(struct sockaddr *)&sendAddress,sizeof(struct sockaddr_in))<0)
	    return -1;
	else
	    return 0;
    }
    
    return socketFD;
}

recvData_t *
receiveTCP(int socketFD)
{
    struct sockaddr_in sendAddress;
    socklen_t len=sizeof(struct sockaddr_in);
    recvData_t * recvData;
    u_size header_size;
    void * ret;
    char * dir;
    short port;
    
    memset(&sendAddress,0x0,sizeof(struct sockaddr_in));

    if(recvfrom(socketFD,&header_size,sizeof(u_size),0,(struct sockaddr *)&sendAddress,&len)<0)
    {
	perror("ACA: ");
	    return NULL;
    }
    printf("header_size: %ld\n",header_size);
    if( (ret=calloc(1,header_size)) == NULL )
		return NULL;
    
    if(recvfrom(socketFD,ret,header_size,0,(struct sockaddr *)&sendAddress,&len)<0)
	    return NULL;
    
    if( (recvData=calloc(1,sizeof(recvData_t)))!=NULL )
    {
	dir=inet_ntoa(sendAddress.sin_addr);
	printf("%s\n",dir);
	port=ntohs(sendAddress.sin_port);
	recvData->port=port;
	strncpy(recvData->dir_inet,dir,DIR_INET_LEN);
	recvData->data=ret;
    }
    return recvData;
    
    /*void * ret;
	u_size *header_size;
	
	if( (header_size = malloc(sizeof(u_size))) == NULL )
		return NULL;
	
	if(recv(socketFD,header_size,sizeof(u_size),0)<0)
	    return NULL;
	
	if( (ret=calloc(1,*header_size)) == NULL )
		return NULL;
	
	if(recv(socketFD,ret,*header_size,0)<0)
	    return NULL;
	
	free(header_size);*/
}

void
FreeRecvData(recvData_t * data)
{
    free(data->data);
    free(data);
}
void 
CloseUDP(int socketFD)
{
	//ver si combiene close o shutdown
	close(socketFD);
	return;
}