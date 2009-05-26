#ifndef _UDPLIB_H_
#define _UDPLIB_H_

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

#include "defines.h"

#define DIR_INET_LEN 18

#define GEN_ERROR -1

#define INV_HOST -2
/*------------DEBUGEO--------*/
typedef struct{
    int num;
    char mensaje[50];
}mensaje_t;
/*------------DEBUGEO--------*/

typedef struct{
    short port;
    char dir_inet[DIR_INET_LEN+10];
    void * data;
}recvData_t;

int prepareUDP(const char * host,const char * port);

int sendUDP(int socketFD,void * data,size_t size,const char * host,const char * port);

recvData_t * receiveUDP(int socketFD);

void FreeRecvData(recvData_t * data);

void CloseUDP(int socketFD);
#endif