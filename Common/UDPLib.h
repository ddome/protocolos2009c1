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

/* Definir el tamaño maximo de los datos a mandar y a recibir.*/
#define UDP_MTU 300

#define DIR_INET_LEN 1024

#define OK 1

#define GEN_ERROR -1

#define INV_HOST -2

typedef struct{
    unsigned short port;
    char dir_inet[DIR_INET_LEN+1];
}host_t;

int prepareUDP(const char * host,const char * port);


/*socketFD: 	Socket descriptor
 *data:		Datos a enviar. Debe ser menor o igual a UDP_MTU.
 *		si no es truncado.
 *dest:		struct del tipo host_t debe contener la informacion
 *		del host destino.
 *Uso de host_t:
 *		host_t host;
 *		host.port=(unsigned shor)atoi("1007");
 *		strncpy(host.dir_inet,"127.0.0.1",DIR_INET_LEN);
*/
int sendUDP(int socketFD,void * data,host_t dest);


/*sender:	puntero a una estructura de tipo host_t. La funcion
 *		devuelve en dicho puntero informacion sobre el
 *		host que envio los datos.
 *
 *El tamaño de los datos que devuelve la funcion es igual a UDP_MTU,
 *sin embardo el tamaño de los datos enviados por el host puede ser
 *menor.
*/
void * receiveUDP(int socketFD,host_t * sender);

void CloseUDP(int socketFD);
#endif