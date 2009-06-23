/* Archivo: config_parser.h
*  ------------------------
*  Interfaz para obtener pares ip-port de los archivos de configuracion.
*/

/* System Includes
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Defines
*/
#define MAXIP 	15
#define MAXPORT 5

/* Types
*/

/* Tipo: ip_state_t
*  ----------------
*  Utilidad para la maquina de estados que determina
*  la validez de direcciones ip.
*/
typedef enum{
	START,
	NUMBER,
	DOT,
	END
} ip_state_t;

/* Tipo: address_t
*  ----------------
*  Estructura de datos que almacena un par ip-puerto
*/
typedef struct{
	char ip[MAXIP + 1];
	char port[MAXPORT + 1];
}  address_t;

/* Tipo: address_t
*  ----------------
*  Estructura de datos que se retorna al usuario. Contiene un array
*  de address_t, y un entero que contiene la cantidad de direcciones
*  retornadas.
*/
typedef struct{
	int count;
	address_t * addresses;
} address_array_t;

/* Interface Functions
*/

/* Funcion: GetAddresses()
*  -----------------------
*  Toma del archivo f los pares ip-puerto que toma de el.
*  El formato del archivo debe ser un csv, por ejemplo:
*  -----------------------------------------------------
*  127.0.0.1;7777
*  162.168.1.2;1234
*  -----------------------------------------------------
*  Es responsabilidad del usuario abrir el archivo previamente,
*  y cerrarlo luego de la llamada.
*  El valor de retorno sera 1 en caso de que el parseo fue
*  exitoso, y 0 en caso contrario.
*/
int
GetAddresses(FILE * f, address_array_t * adds);


