#ifndef HASH_H_
#define HASH_H_

typedef struct hashCDT * hashADT;

typedef void *  hashElementT;
typedef int     (* hashCmp) (hashElementT, hashElementT);
typedef int     (* hashSaveItem) (FILE*,hashElementT);
typedef hashElementT (* hashLoadItem) (FILE*);


/* Definicion prototipo funcion de hasheo, recibe el elemento a hashear y la dimension
 * actual de la tabla */

typedef int (* hashKey) (hashElementT, int);

/* Crea una instancia de una tabla de hash */

hashADT NewHash(int esize, hashCmp hcmp, hashKey hkey,hashSaveItem hsave,hashLoadItem hload);

hashADT LoadHashTable(char *path,int esize, hashCmp hcmp, hashKey hkey,hashSaveItem hsave,hashLoadItem hload);

int SaveHashTable(hashADT table,char *path); 

/* Inserta un elemento en la tabla */

int HInsert(hashADT hash, hashElementT data);

/* Borra un elemento de la tabla */

int     HDelete(hashADT hash, hashElementT data);

/* Busca un elemento en la tabla y retorna su posicion en la misma */

int Lookup(hashADT hash, hashElementT data);

/* Retorna el tamanio de la tabla de hash */

int GetSlots(hashADT hash);

/* Setea el iterador, usar antes de ejecutar GetNextElement */

int SetHashBegin(hashADT hash);

/* Devuelve el proximo elemento en la tabla */

int GetNextElement(hashADT hash, hashElementT * element);

/* Dada una posicion, devuelta por lookup, retorna el elemento en dicha posicion de la tabla */

hashElementT GetHElement(hashADT hash, int pos);

/* Destruye toda la tabla */

void FreeHash(hashADT hash);

int     HTableIsEmpty(hashADT hash);

/* Devuelve la primer posicion vacia de la tabla */
unsigned int GetFreePosition(hashADT hash);

#endif /*HASH_H_*/
