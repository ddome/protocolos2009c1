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
sendUDP(int socketFD,void * data,host_t dest)
{
    struct sockaddr_in sendAddress;
    struct hostent	*phe;
    
    memset(&sendAddress, 0x0, sizeof(sendAddress));
    sendAddress.sin_family=AF_INET;
    sendAddress.sin_port=htons(dest.port);

    if ( (phe = gethostbyname(dest.dir_inet)) )
	    memcpy(&sendAddress.sin_addr, phe->h_addr, phe->h_length);
    else if ((sendAddress.sin_addr.s_addr = inet_addr(dest.dir_inet))==INADDR_NONE)
	    socketFD=INV_HOST;
    
    if(socketFD>=0)
    {
	if(sendto(socketFD,data,UDP_MTU,0,(struct sockaddr *)&sendAddress,sizeof(struct sockaddr_in))<0)
	{
	    return -1;
	}
	else
	{
	    return 0;
	}
    }
    
    return socketFD;
}

void *
receiveUDP(int socketFD,host_t * sender)
{
    struct sockaddr_in sendAddress;
    socklen_t len=sizeof(struct sockaddr_in);
    void * ret;
    char * dir;
    short port;
    
    if( (ret=calloc(1,UDP_MTU))==NULL )
	return NULL;

    memset(&sendAddress,0x0,sizeof(struct sockaddr_in));
    if(recvfrom(socketFD,ret,UDP_MTU,0,(struct sockaddr *)&sendAddress,&len)<0)
	return NULL;
    

    dir=inet_ntoa(sendAddress.sin_addr);
    printf("%s\n",dir);
    port=ntohs(sendAddress.sin_port);
    sender->port=port;
    strncpy(sender->dir_inet,dir,DIR_INET_LEN);


    return ret;
}

void 
CloseUDP(int socketFD)
{
	//ver si combiene close o shutdown
	close(socketFD);
	return;
}

void
setSocketTimeoutUDP(int socket,int seconds)
{
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,  sizeof(tv));
}