#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tree.h"


#define OK 		  1

typedef struct nodeCDT * nodeADT;

typedef struct nodeCDT {
    char * nombre;
    struct nodeCDT * next;
    struct nodeCDT * hijo;
    int (*func) (scannerADT scanner, void * data);
} nodeCDT;

typedef struct treeCDT {
    struct nodeCDT * nodes;
} treeCDT;


static int Insert (nodeADT * nodes, scannerADT scanner, int (*function) (scannerADT scanner, void * data));

static int Read (nodeADT * nodes, scannerADT scanner, void * data);

treeADT
NewTree(void)
{
    treeADT tree;
    if ( (tree = calloc(sizeof(treeCDT), 1)) == NULL )
        return NULL;
    return tree;
}


static void
FreeNode( nodeADT node )
{
    free( node->nombre );
    if( node->next != NULL )
        FreeNode( node->next );
    if( node->hijo != NULL )
        FreeNode( node->hijo );
    free( node );
}

void
FreeTree( treeADT tree )
{
    FreeNode( tree->nodes );
    free( tree );

}

static int
Insert(nodeADT * nodes, scannerADT scanner, int (*func) (scannerADT scanner, void * data))
{
    char * token;
    token = ReadToken(scanner);
    if ( *nodes == NULL ) 
    {
        if ( (*nodes = malloc(sizeof(nodeCDT))) == NULL )
        {
            free(token);
            return -1;
        }
        if ( *token == 0 ) 
        {
            (*nodes)->nombre = NULL;
            (*nodes)->next = NULL;
            (*nodes)->hijo = NULL;
            (*nodes)->func = func;
            return 0;
        }
        (*nodes)->nombre = token;
        (*nodes)->hijo = NULL;
        (*nodes)->next = NULL;
        (*nodes)->func = NULL;

        Insert(&(*nodes)->hijo, scanner, func);
    }
    else if ( strcmp((*nodes)->nombre, token) != 0 )
    {
        SaveToken(scanner, token);
        Insert(&(*nodes)->next, scanner, func);
    }
    else if ( strcmp((*nodes)->nombre, token) == 0 )
    {
        Insert(&(*nodes)->hijo, scanner, func);
    }

    return 0;
}

static int
Read( nodeADT * nodes, scannerADT scanner,void * data)
{
    char * token;

    token = ReadToken( scanner );

    if ( (*nodes)->func != NULL )
    {
        SaveToken(scanner, token);
        return (*nodes)->func(scanner, data);
    }

    if ( *token == 0 )
    {
        if ( (*nodes)->func == NULL )
          return __INVALID_COMMAND__;
        else
	    return (*nodes)->func(scanner, data);
    }

    if ( *nodes == NULL || (*nodes)->nombre == NULL )
    {
        free(token);/*ACA*/
        return __INVALID_COMMAND__;
    }
    else if ( strcmp((*nodes)->nombre, token) == 0 )	
    {
        if ( (*nodes)->hijo == NULL )
        {
            free(token);/*ACA*/	
            return __INVALID_COMMAND__;
        }

        return Read (&(*nodes)->hijo, scanner, data);
    }
    else if ( strcmp((*nodes)->nombre, token) != 0 )
    {
        if ( (*nodes)->next == NULL )		
	{
            free(token);/*ACA*/
            return __INVALID_COMMAND__;
        }
        SaveToken(scanner, token);
        return Read (&(*nodes)->next, scanner, data);

    }
    return __INVALID_COMMAND__;
}

int
InsertExpression( treeADT tree, char * expression, int (*func) (scannerADT scanner, void * data) )
{
    scannerADT scanner;
    scanner = NewScanner();

    SetScannerString(scanner, expression);

    Insert(&tree->nodes, scanner, func);
    FreeScanner(scanner);
    return 0;
}

int
ReadExpression(treeADT tree, char * expression, void * data)
{
    int res;
    scannerADT scanner;
    scanner = NewScanner();

    SetScannerString(scanner, expression);

    res =Read(&tree->nodes, scanner, data);
    FreeScanner(scanner);
    return res;
}





