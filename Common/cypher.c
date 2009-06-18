#include "cypher.h"

int setKey(char desKey[CYPHER_SIZE], char * password);
int encriptar(char * resp, char * original,int size, char * desKey);
int desencriptar(char * resp, char * encriptado,int size, char * desKey);


char*
Cypher(char * original,int size,char * password)
{
    char * resp;
    char desKey[CYPHER_SIZE];
      
    if(setKey(desKey,password)<0)
	return NULL;

    if((resp=malloc(size+(CYPHER_SIZE-size%CYPHER_SIZE)))==NULL)
    {
	fprintf(stderr,"Error fatal de memoria.\n");
	exit(1);
    }
    encriptar(resp,original,size,desKey);
    return resp;
}

char*
Decypher(char * encriptado,int size,char * password)
{
    char * resp;
    char desKey[CYPHER_SIZE];
    if(setKey(desKey,password)<0)
	return NULL;
    if((resp=malloc(size))==NULL)
    {
	fprintf(stderr,"Error fatal de memoria.\n");
	exit(1);
    }
    desencriptar(resp,encriptado,size,desKey);
    return resp;
}


int
setKey(char desKey[CYPHER_SIZE], char * password)
{
    int longPass=0;
    int i=0;
    if(password==NULL)
	return CYPHER_ERR;
    longPass=strlen(password);
    if(longPass < CYPHER_SIZE)
    {
	strcpy(desKey,password);
	for(i=strlen(desKey);i<CYPHER_SIZE;i++)
	{
	    desKey[i]=0;
	}
    }
    else if(longPass > CYPHER_SIZE)
    {
	memcpy(desKey,password,CYPHER_SIZE);
    }
    else
    {
	printf("Password de %d digitos y dice %s\n",strlen(password),password);
	memcpy(desKey,password,CYPHER_SIZE);
	printf("En deskey dice %s\n",desKey);
    }
    return 1;
}

int
encriptar(char * resp, char * original,int size, char * desKey)
{  
    int i=0;
    int j=0;
    int cant;
    char aux[CYPHER_SIZE];
    cant=size+(CYPHER_SIZE-size%CYPHER_SIZE);
    for(i =0 ; i < size-size%CYPHER_SIZE ; i=i+CYPHER_SIZE)
    {
	des_encipher((unsigned char *)&original[i],(unsigned char *)&resp[i],(unsigned char *) desKey);
    }
    if(size%CYPHER_SIZE!=0)
    {
	memcpy(aux,&original[i],size%CYPHER_SIZE);
	for(j=size% CYPHER_SIZE;j<CYPHER_SIZE; j++)
	{
	    aux[j]=0;
	}
	des_encipher((unsigned char *)aux,(unsigned char *)&resp[i],(unsigned char *)desKey);
    }
    return 1;
}




int
desencriptar(char * resp, char * encriptado,int size, char * desKey)
{
    char * buffer;
    int i=0;
    int cant;
    int sizeEncriptado= size + ((size%CYPHER_SIZE==0)?0: (CYPHER_SIZE - size % CYPHER_SIZE));
	
    if((buffer=malloc(sizeEncriptado))==NULL)
    {
	fprintf(stderr,"Error fatal de memoria.\n");
	exit(1);
    }
    for(i =0 ; i < sizeEncriptado ; i=i+CYPHER_SIZE)
    {
	des_decipher((unsigned char *)&encriptado[i],(unsigned char *)&buffer[i],(unsigned char *)desKey);
    }
    memcpy(resp,buffer,size);
    free(buffer);
    return 1;
}


