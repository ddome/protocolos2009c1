/*
 *  defines.h
 *  MovieStoreServer
 */

/* Aclaracion: cualquier estado de error definirlo como menor a 0 */


#ifndef __DEFINES_H_
#define __DEFINES_H_

/* DEFINES DE USO GENERAL */

typedef enum status { FATAL_ERROR=-2, ERROR=-1, OK=1 } status;

typedef enum boolean { FALSE=0, TRUE=1 } boolean;

typedef unsigned long u_size;

typedef char * string;

#endif



