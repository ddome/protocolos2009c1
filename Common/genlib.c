/*
 *  genlib.c
 */

#include "genlib.h"

string 
Concat( string s1, string s2 )
{
	string aux;
	
	if( (aux=malloc( (strlen(s1)+strlen(s2)+1)*sizeof(char) )) == NULL )
		return NULL;	
	strcpy(aux,s1);
	strcat(aux,s2);
	return aux;
}

string
CreateString( string copy )
{
	string aux;
	
	if( ( aux=malloc( (strlen(copy)+1)*sizeof(char) ) ) == NULL )
		return NULL;
	strcpy(aux,copy);
	return aux;
}

char*
CopyString(char * string)
{
    char * newStr;
    int len;
    if(string==NULL)
	return NULL;
    len=strlen(string);
    
    if( (newStr=calloc( len, sizeof(char) ))==NULL )
    {
	fprintf(stderr,"No hay memoria suficiente para encolar.\n");
	return NULL;
    }
    strncpy(newStr, string, len);
    
    return newStr;
}
