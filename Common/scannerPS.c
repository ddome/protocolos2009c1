/*
* Archivo: scanner.c
* ------------------
* Este archivo implementa la interfaz scanner.h
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scannerPS.h"

#define EMPTY_STRING ""

#define ES_SEPARADOR(c) ((c) == '\0' || (c) == '\n')

/*
 * Tipo: scannerCDT
 * ----------------
 * Esta estructura es la representacion concreta del tipo scannerADT, que es
 * exportado por esta interfaz. Si bien los detalles de la representacion
 * son inaccesibles al cliente, consisten en los siguientes campos:
 *
 * str         -- Copia del string enviado a SetScannerString
 * len         -- Cantidad de caracteres del string, guardada aqui por
 *                cuestiones de eficiencia
 * cp          -- Caracter actual dentro del string
 * savedToken  -- String guardado por SaveToken. Null indica que no hay ninguno
 * spaceOption -- Guarda la option de ignorar espacios o no
*/

struct scannerCDT
{
	char * str;
	int len;
	int cp;
	char * savedToken;
	separatorOptionT separatorOption;
};

/* Prototipos de funciones privadas */

static void SkipSeparators(scannerADT scanner);
static int ScanToEndOfIdentifier(scannerADT scanner);
static void Error(char * msg);
static char * SubString(char * s, int p1, int p2);

/* Funciones exportadas */

scannerADT
NewScanner(void)
{
	scannerADT scanner;
	if((scanner = malloc(sizeof(struct scannerCDT))) == NULL)
	{
		Error("\n Error al alocar memoria\n");
		return NULL;
	}
	scanner->str = NULL;
	scanner->separatorOption = IgnoreSeparators;
	return scanner;
}

void
FreeScanner(scannerADT scanner)
{
	if(scanner->savedToken!=NULL)
	    free(scanner->savedToken);
	if(scanner->str != NULL)
	    free(scanner->str);
	free(scanner);
}

void
SetScannerString(scannerADT scanner, char * str)
{
	int len;
	if(scanner->str != NULL)
		free(scanner->str);
	len = strlen(str);
	if( (scanner->str = malloc((len+1)*sizeof(char))) == NULL)
	{
		Error("\nError al alocar memoria, no se pudo setear el string\n");
		return;
	}
	strcpy(scanner->str, str);
	scanner->len = len;
	scanner->cp = 0;
	scanner->savedToken = NULL;
}

char *
ReadToken(scannerADT scanner)
{
	char ch;
	char * token;
	int start, finish;

	if(scanner->str == NULL)
	{
		Error("\nAun no se ha seteado un string por medio de SetScannerString\n");
		return EMPTY_STRING;
	}
	
	if(scanner->savedToken != NULL)
	{

                token = scanner->savedToken;
                scanner->savedToken=NULL ;
                return token;
        
	}
	
	if(scanner->separatorOption == IgnoreSeparators)
		SkipSeparators(scanner);
	
	start = finish = scanner->cp;
	
	if(start >= scanner->len)
		return EMPTY_STRING;
	
	ch = scanner->str[scanner->cp];
	
	if(!ES_SEPARADOR(ch))
	{
		finish = ScanToEndOfIdentifier(scanner);
	}
	else
	{
		scanner->cp++;
	}
	
	return SubString(scanner->str,start,finish);
}

int 
MoreTokensExist(scannerADT scanner)
{
	if(scanner->str == NULL)
	{
		Error("\nAun no se ha seteado un string por medio de SetScannerString\n");
		return 0;
	}
	if(scanner->savedToken != NULL)
		return 1;
	if(scanner->separatorOption == IgnoreSeparators)
		SkipSeparators(scanner);
	return (scanner->cp < scanner->len);
}

void
SaveToken(scannerADT scanner, char * token)
{
	if(scanner->str == NULL)
	{
		Error("\nAun no se ha seteado un string por medio de SetScannerString\n");
		return;
	}
	if(scanner->savedToken != NULL)
	{
		Error("\nYa se ha grabado un token");
		return;
	}
	if((strlen(token)>1) && !ES_SEPARADOR(token[0]) )
        scanner->savedToken=token;
}

void
SetScannerSeparatorOption(scannerADT scanner, separatorOptionT option)
{
	scanner->separatorOption = option;
}

separatorOptionT
GetScannerSeparatorOption(scannerADT scanner)
{
	return scanner->separatorOption;
}

/* Funciones Privadas */

/*
* Funcion: SkipSpaces
* Uso: SkipSpaces(scanner);
* -------------------------
* Avanza la posicion del scanner hasta que terminen los espacios.
*/

static void
SkipSeparators(scannerADT scanner)
{
	while(ES_SEPARADOR(scanner->str[scanner->cp]) 
                            && scanner->str[scanner->cp]!='\0')
		scanner->cp++;
}

/*
 * Funcion: ScanToEndOfIdentifier
 * Uso: finish = ScanToEndOfIdentifier(scanner);
 * ---------------------------------------------
 * Avanza el scanner hasta el final de la secuencia de letras o dogitos que
 * forman un identificador. El valor de retorno es el ultimo caracter del
 * identificador; el valor del �ndice cp es el primer caracter siguiente a
 * dicho valor de retorno.
*/

static int
ScanToEndOfIdentifier(scannerADT scanner)
{
	while(!ES_SEPARADOR(scanner->str[scanner->cp]))
		scanner->cp++;
	return (scanner->cp - 1);
}

static void
Error(char * msg)
{
	fprintf(stderr, msg);
}

static char * 
SubString(char * s, int p1, int p2)
{
    int len;
    char * result;

    if (s == NULL) 
		return NULL;
	
    len = strlen(s);
    
	if (p1 < 0) 
		p1 = 0;
    if (p2 >= len) 
		p2 = len - 1;
    
	len = p2 - p1 + 1;
    
	if (len < 0) 
		len = 0;
    
	if((result = malloc(len+1))==NULL)
		return NULL;
		
	strncpy(result, s + p1, len);
    
	result[len] = '\0';
    
	return result;
}

