#ifndef _CYPHER_
#define _CYPHER_

#include "./des/include/bit.h"
#include "./des/include/encrypt.h"
#include<string.h>
#include<stdlib.h>



/*ATENCION: Maxima longitud del password. De ser excedida se truncara el string.
 *Es decir que el password "12345678" y "12345678a" son lo mismo.
*/
#define CYPHER_SIZE 8



/*Retorno de las funciones si no se produjo ningun error.*/
#define OK 0
/*Retorno de las funciones si se produjo algun error.*/
#define ERROR_CYPHER -1



/* Encripta un secuencia de bytes dados.
 *
 *	resp:	Almacena los datos ya encriptados en *resp. La funcion se
 *		se encarga de alocar el espacio suficiente. El usuario es
 *		responsable de liberar dicho espacio.
 *	msg:	Los datos a encriptar.
 *	size:	El tamaño de los datos a encriptar.
 *	passwd:	Password para encriptar los datos.
*/
int Cypher(char ** resp,const char * msg,size_t size,const char * passwd);



/* Desencripta un secuencia de bytes encriptados previamente con la funcion
 *Cypher.
 *
 *	resp:	Almacena los datos desencriptados en *resp. La funcion se
 *		se encarga de alocar el espacio suficiente. El usuario es
 *		responsable de liberar dicho espacio.
 *	msg:	Los datos a desencriptar.
 *	size:	El tamaño de los datos a desencriptar. El tamaño original.
 *		No el tamaño de los datos ya encriptados.
 *	passwd:	Password para desencriptar los datos. Debe coincidir con el
 *		que se uso para encriptarlos.
*/
int Decypher(char ** resp,const char * msg,size_t size,const char * passwd);

#endif
