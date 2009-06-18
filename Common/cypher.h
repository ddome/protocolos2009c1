#ifndef _CYPHER_H
#define _CYPHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./des/include/bit.h" 
#include "./des/include/encrypt.h" 

#define CYPHER_SIZE 8
#define CYPHER_ERR -1

char * Cypher(char * original,int size,char * password);

char * Decypher(char * original,int size,char * password);

#endif



















