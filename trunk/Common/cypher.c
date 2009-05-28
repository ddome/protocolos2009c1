#include "cypher.h"



/*Funcion que trunca el password en caso de ser mas largo y rellena con 0 el
 *sobrante en caso de que el password tenga una longitud menor a 8. Esto se
 *hace para estar seguros de no levantar basura.
 */
static char *
FillPasswd(const char * passwd)
{
    char * ret;

    if( (ret=calloc(1,CYPHER_SIZE))==NULL )
	return NULL;
    
    /*Solo tomo los primeros 8 caracteres (bytes) del password.*/
    memmove(ret,passwd,CYPHER_SIZE);
    
    return ret;
}



int
Cypher(char ** resp,const char * msg,size_t size,const char * passwd)
{
    unsigned long i,j;
    char * newPasswd, * ret;
    char aux[CYPHER_SIZE];
    
    if(resp==NULL || msg==NULL || size <=0 || passwd==NULL )
	return ERROR_CYPHER;
    
    if( (newPasswd=FillPasswd(passwd))==NULL )
	return ERROR_CYPHER;
    
    /*Reservo espacio para guardar los datos encritados. Como la implemetacion
     *de DES encripta de a 8 bytes debo completar para que el total de espacio
     *disponible para guardar la respuesta sea multiplo de 8.
     */
    if( ( ret=calloc(1,size+(CYPHER_SIZE-size%CYPHER_SIZE)) )==NULL )
    {
	free(newPasswd);
	return ERROR_CYPHER;
    }

    /*Encripto de a 8 bytes. En caso de de que el tamano de los datos no sea
     *multiplo de 8 encripto el maximo multiplo de 8 posible.
     */
    for(i =0 ; i < size-(size%CYPHER_SIZE) ; i=i+CYPHER_SIZE)
    {
	des_encipher((unsigned char *)&msg[i],
		      (unsigned char *)&ret[i], (unsigned char *)newPasswd);
    }
    
    /*Si el tamano de los datos no era multiplo de 8 encripto el fragmento
     *restante
     */
    if(size%CYPHER_SIZE!=0)
    {
	/*Inicializo aux y copio los datos que faltaron encriptar. Esto se
	 *hace para estar seguros de no levantar basura al ser lo restante
	 *menor a 8 bytes.
	 */
	for(j=size% CYPHER_SIZE;j<CYPHER_SIZE; j++)
	{
	    aux[j]=0;
	}
	memcpy(aux,&msg[i],size%CYPHER_SIZE);
	des_encipher((unsigned char *)aux,
		      (unsigned char *)&ret[i],(unsigned char *)newPasswd);
    }
    
    *resp=ret;
    free(newPasswd);
    
    return OK;
}



int
Decypher(char ** resp,const char * msg,size_t size,const char * passwd)
{
    unsigned long i,cypheredSize;
    char * newPasswd, * ret,*aux;
    
    if(resp==NULL || msg==NULL || size <=0 || passwd==NULL )
	return ERROR_CYPHER;
    
    if( (newPasswd=FillPasswd(passwd))==NULL )
	return ERROR_CYPHER;

    /*Reservo espacio para ir guardando los datos a medida que desencriptando
     *de a 8 bytes. Debo reservar el multiplo de 8 mas proximo a size y mayor
     *a este. Como la imprimetacion de DES desencripta de a 8 bytes debo
     *completar para que el total de espacio disponible para guardar la
     *respuesta sea multiplo de 8.
     */
    cypheredSize= size + ((size%CYPHER_SIZE==0)?0: (CYPHER_SIZE - size % CYPHER_SIZE));
    
    if( (ret=calloc(1,cypheredSize))==NULL )
    {
	free(newPasswd);
	return ERROR_CYPHER;
    }
    
    /*Desencripto los datos.*/
    for(i =0 ; i < cypheredSize ; i=i+CYPHER_SIZE)
    {
	des_decipher((unsigned char *)&msg[i],(unsigned char *)&ret[i],(unsigned char *)newPasswd);
    }
    
    /*Reservo el espacio justo para la respuesta ya que para realizar el paso
     *anterior se reservo espacio de mas. Luego copio la respuesta a su nueva
     *ubicacion.
     */
    if( (aux=calloc(1,cypheredSize))==NULL )
    {
	free(newPasswd);
	free(ret);
	return ERROR_CYPHER;
    }
    memmove(aux,ret,size);
    
    *resp=aux;
    free(newPasswd);
    free(ret);
    
    return OK;
}
