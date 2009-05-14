/*
 *  debug.c
 *  MovieStoreServer
 */


#include <stdio.h>

#include "debug.h"


void
debug_number(char *tag, int number)
{
	char aux[50];
	
	sprintf(aux,"%s: %d",tag,number);
	fopen(aux,"w+");	
}

void
debug_llego(void)
{
	fopen("Llego! bien pibe","w+");
}
