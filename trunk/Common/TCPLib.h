#ifndef _TCPLIB_H_
#define _TCPLIB_H_

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
/*
typedef struct{
    int num;
    char mensaje[100];
}packet_t;*/

/*Especifica si el socket se debe crear para escuchar conexiones o para escuchar
 * conexiones o para conectarse a un cliente para la tranferencia de archivos.*/
typedef enum{prepareServer=0,connectClient} type_t;

/*Le mandas el IP y el puerto como strings. En type pone prepareServer.
 te devuelve un file descriptor. Tiene que ser mayor a 0, sino es error.*/
int prepareTCP(const char * host,const char * port,type_t type);

/*Le mandas el file descriptor q te devolvio prepareTCP y en max_queue_len
 ponele 10. Devulve 0 si salio todo bien y un numero negativo si hubo error.*/
int listenTCP(int socketFD,int max_queue_len);

/*Le indicas a que ip y puerto te queres conectar. Si la conexion
 salio bien te devulve un file descriptor. Tiene que ser mayor a 0, sino es error.
 El file descriptor se usa con sendTCP y receiveTCP*/
int connectTCP(const char * host,const char * port);

/*Le envias el file descriptor que te dio pepareTCP y se queda blocqueado hasta que
 llegue una nueva solicitud de conexion. Es bloqueante. Cuando alguien pide conectarse 
 esta fucion devuelve un NUEVO file descriptor que usas para sendTCP y receiveTCP.*/
int acceptTCP(int socketFD);

int sendTCP(int socketFD,void * data,size_t size);

/*Es necesario liberar lo q devuelve.*/
void * receiveTCP(int socketFD);

/*Cierra una conexion determinada.*/
void closeTCP(int socketFD);

#endif
