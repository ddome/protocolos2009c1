/* Archivo: config_parser.c
*  ------------------------
*  Herramienta para obtener pares ip-port de los archivos de configuracion.
*/

/* Project Includes
*/
#include "config_parser.h"

/* Static Prototypes
*/

static char * ReadLine( FILE * inputFile );
static int IsNumber(char c);
static int IsValidPort(char * token);
static int IsValidIp(char * token);
static int GetAddressData(char * aux, address_t * add);

/* Interface Implementation
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
GetAddresses(FILE * f, address_array_t * adds)
{
	address_t address;
	address_t auxAddress;
	address_t * auxPtr;
	char * aux;

	adds->count = 0;
	adds->addresses = NULL;
	if(f == NULL)
	{
		return 0;	
	}

	while((aux = ReadLine(f))!=NULL)
	{
		if(!GetAddressData(aux, &address))		
		{
			return 0;
		}
		memcpy(&auxAddress, &address, sizeof(address_t));
		auxPtr = (void*)realloc(adds->addresses, (adds->count+ 1)* sizeof(address_t));
		if(auxPtr == NULL)
		{
			return 0;
		}
		adds->addresses = auxPtr;
		memcpy(&(adds->addresses[adds->count]), &address, sizeof(address_t));

		(adds->count)++;
		free(aux);
		
	}
	return 1;
}

/* Static Functions
*/

/* Funcion: GetAddressData()
*  -------------------------
*  Extrae la direccion ip y el puerto de una determinada
*  linea del archivo csv.
*/
static int
GetAddressData(char * aux, address_t * add)
{
	char * token;
	token = strtok(aux, ";");

	if(token==NULL){
		return 0;
	}

	/* Direccion de ip */
	if(!IsValidIp(token)){
		return 0;
	}
	strcpy(add->ip, token);	

	token = strtok(NULL,";");

	if(token == NULL){
		return 0;
	}

	/* Puerto */
	if(!IsValidPort(token)){
		return 0;
	}
	strcpy(add->port, token);

	return 1;
}

/* Funcion:IsValidIp()
*  -------------------
*  Devuelve 1 en caso de que token corresponda a una direccion ip, 
*  y 0 en caso contrario
*/
static int
IsValidIp(char * token)
{
	ip_state_t state = START;
	int numCount = 0, count = 0, i, len;
	char c;
	
	len = strlen(token);
	for(i = 0; i < len; i++)
	{
		c = *(token + i);
		switch(state)
		{
			case START:
				if(!IsNumber(c))
					return 0;
				state = NUMBER;
				count = 1;
				numCount = 1;
				break;
			case NUMBER:
				if(IsNumber(c)){
					if(numCount==3)
						return 0;
					numCount++;
				}
				else if(c == '.'){
					state = DOT;
					numCount = 0;
				}
				else{
					return 0;
				}					
				break;
			case DOT:
				if(!IsNumber(c)){
					return 0;
				}
				if(count > 4){
					return 0;
				}
				numCount = 1;
				count++;
				state = NUMBER;
				break;
			case END:
				break;
		}
	}
	return 1;
}

/* Funcion:IsValidPort()
*  -------------------
*  Devuelve 1 en caso de que token corresponda a un puerto valido, 
*  y 0 en caso contrario
*/
static int
IsValidPort(char * token)
{
	int i;
	int len = strlen(token);
	if(len == 0 || len > 4){
		return 0;
	}
	for(i = 0; i<len; i++){
		if(!IsNumber(*(token + i))){
			return 0;
		}
	}
	return 1;
}

/* Funcion:IsNumber()
*  -------------------
*  Devuelve 1 en caso de que el caracter c corresponda a un numero, 
*  y 0 en caso contrario
*/
static int
IsNumber(char c)
{
	return c <= '9' && c >= '0';
}

/* Funcion:ReadLine()
*  -------------------
*  Toma una linea a partir del cursor actual del archivo inputFile.
*/
static char *
ReadLine( FILE * inputFile )
{
    char line[255+ 1];
    char * resp;
    int len;
	
    if( fgets( line, 255, inputFile ) ==NULL )
		return NULL;
    if( ((len = strlen( line )) == 0) || ((len = strlen( line )) == 1 ) )
        return NULL;
    line[len-1] = '\0';
    line[len] = '\0';
	
    resp = malloc( (len + 1) * sizeof(char) );
    strncpy(resp, line, len + 1);
	
    return (resp);
}



