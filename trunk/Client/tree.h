#ifndef _TREE_H
#define _TREE_H

#include "scannerADT.h"

#define __INVALID_COMMAND__ -3

typedef struct treeCDT * treeADT;

treeADT NewTree(void);

void FreeTree( treeADT tree );


int InsertExpression( treeADT tree, char * expression, int (*func) (scannerADT scanner, void * data) );

int ReadExpression(treeADT tree, char * expression, void * data);

#endif
