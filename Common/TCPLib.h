#ifndef TCPLIB_H_
#define TCPLIB_H_

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>


/*General error*/
#define GEN_ERROR -1
/*The specified host is invalid*/
#define INV_HOST -2
/*The given address is already in use.*/
#define ADD_IN_USE -3

/*Connection has been aborted.*/
#define CONN_ABORTED -15
/*Firewall rules forbid connection.*/
#define FIREWALL_PERM -16

/*Connection reset by peer*/
#define CONN_RST_PEER -25
/*Invalid arguments*/
#define INV_ARGS -26
/*Message size error.*/
#define MSG_SIZE_ERR -27



/*Especifica si el socket se debe crear para escuchar conexiones o para escuchar
 * conexiones o para conectarse a un cliente para la tranferencia de archivos.*/
typedef enum{prepareServer=0,connectClient} type_t;

#define SEND_FLAGS MSG_DONTWAIT

int prepareTCP(const char * address,int port,type_t type);

int listenTCP(int socketFD,int max_queue_len);

int connectTCP();

int acceptTCP(int socketFD);

int sendTCP(int socketFD,void * data,size_t size);

void * receiveTCP();

void closeTCP(int socketFD);

#endif /*TCPLIB_H_*/
