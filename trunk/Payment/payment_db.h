#ifndef __PAYMENT_DB_H__
#define __PAYMENT_DB_H__


#include <stdio.h>
#include "paymentServerLib.h"

int psComp( void * v1, void *v2 );

int psHash( void *v, int size );

int psSave(FILE *fd,void *data);

void * psLoad(FILE *fd);

#endif
