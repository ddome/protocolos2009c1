#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hashADT.h"

#define TABLE_INIT_SIZE 13 /* Numero primo */
#define LIMIT           0.6

typedef enum {PHYSICAL, OCCUPIED, LOGICAL} statusT;

typedef struct
        {
        hashElementT    data;
        statusT                 status;
        } htableT;

struct hashCDT
{
        htableT *               htable;
        int                             datasize;
        int                             tabledim;
        int                             qtty;
        htableT *               actual;
        int                     index;
        hashCmp                 hcmp;
        hashKey                 hkey;
		hashSaveItem			hsave;
		hashLoadItem			hload;
		
};

static int InitTable(hashADT hash);
static int Rehash(hashADT hash);        

hashADT
NewHash(int esize, hashCmp hcmp, hashKey hkey,hashSaveItem hsave,hashLoadItem hload)
{
        hashADT aux;
       
        if ( esize <= 0 || hcmp == NULL || hkey == NULL )
                return NULL;
       
        if ( (aux = calloc(1, sizeof(struct hashCDT))) == NULL )
        {
                fprintf(stderr, "Not enough memory to create hash table\n");
                return NULL;
        }
       
        aux -> datasize = esize;
        aux -> hcmp = hcmp;
        aux -> hkey = hkey;
 		aux -> hsave = hsave;
		aux -> hload = hload;
		    
        return aux;
}

int
HInsert(hashADT hash, hashElementT data)
{
        int firstLogic = -1;
        int iter = 0;
        int pos, ok = 1;
        int equal;
        double load;
        htableT * htable;
       
        if ( hash == NULL  )
        {
                fprintf(stderr, "Invalid arguments to function HInsert\n");
                return 0;
        }
       
        /* Si es la primera insercion sobre la tabla, se inicia la tabla */
        if ( HTableIsEmpty(hash) )
                if ( (hash -> tabledim = InitTable(hash)) == 0 )
                        return 0;
       
        /* Obtiene la clave hasheada */
        pos = hash -> hkey(data, GetSlots(hash));
       
        htable = hash -> htable;
       
        /* Mientras no se recorra toda la tabla y (la posicion hasheada este ocupada por
         * un elemento distinto al que se quiere insertar o esa ranura tenga una baja logica
         * continuar buscando una posicion vacia.
         */
        while ( iter != GetSlots(hash) && ( (htable[pos].status == OCCUPIED && hash -> hcmp(htable[pos].data, data) != 0) || htable[pos].status == LOGICAL ) )
        {
                /* Si en el recorrido en busca de una celda vacia se encuentra una baja logica
                 * y es la primera, se almacena dicha posicion para insertar alli mas tarde
                 */
                if (hash -> htable[pos].status == LOGICAL)
                        if (firstLogic == -1)
                                firstLogic = pos;
               
                pos = (pos + 1) % hash -> tabledim;
        }
       
       
       
        /* Elemento repetido, no se inserta */
        if (htable[pos].status == OCCUPIED)
        {
                equal = hash -> hcmp(htable[pos].data, data);
               
                if (equal == 0)
                        return 0;
        }
       
        else
        {
                /* Se recorrio toda la tabla, no hay mas lugar, no se puede insertar */
                if (iter == hash -> tabledim)
                        return 0;
               
                else
                {
                        /* Se encontro lugar, se insertar en la primera baja logica si es que la hubo
                         * o en la celda vacia */
                        if (firstLogic != -1)
                        {
                                htable[firstLogic].status = OCCUPIED;
                               
                                /* no reserva memoria pues en baja logica no se borra al elemento */                            
                                memcpy(htable[firstLogic].data, data, hash -> datasize);
                        }
                        else
                        {
                                hash -> htable[pos].status = OCCUPIED;
                                if ( (htable[pos].data = malloc(hash -> datasize)) == NULL )
                                {
                                       
                                        return 0;
                                }
                                memcpy(htable[pos].data, data, hash -> datasize);
                        }
                }
        }
       
        hash -> qtty++;
       
        /* Se hace el chequeo para saber si es necesario agrandar la tabla segun el factor de
         * carga
         */
        load = (double)hash -> qtty / GetSlots(hash);
       
        if (load > LIMIT)
                ok = Rehash(hash);
       
        return ok;
}

unsigned int
GetFreePosition(hashADT hash)
{
	unsigned int pos = 0;
	while( pos<hash->tabledim && hash->htable[pos++].status == OCCUPIED  );
	return pos;
}

int
Lookup(hashADT hash, hashElementT data)
{
        int iter = 0;
        int pos;
        htableT * htable;
       
        if ( hash == NULL || data == NULL )
        {
                fprintf(stderr,"Lookup@hashADT.c - Invalid arguments\n");
                return -1;
        }
       
        if (HTableIsEmpty(hash))
        {
                return -1;
        }
       
        pos = hash -> hkey(data, GetSlots(hash));
       
        htable = hash -> htable;
       
        while ( iter != GetSlots(hash) && ( (htable[pos].status == OCCUPIED && hash -> hcmp(htable[pos].data, data) != 0 ) || htable[pos].status == LOGICAL ) )
                pos = (pos + 1) % GetSlots(hash);
       
       
        if (htable[pos].status == OCCUPIED && hash -> hcmp(htable[pos].data, data) == 0 )
                return pos;
       
        return -1;
}

int
GetSlots(hashADT hash)
{
        if ( hash == NULL )
        {
                fprintf(stderr, "Invalid arguments to function GetSlots\n");
                return -1;
        }
       
        return hash -> tabledim;
}

int
GetNextElement(hashADT hash, hashElementT * element)
{
        int     aux = 1;
       
        if ( hash == NULL || HTableIsEmpty(hash) || hash -> index < 0 || hash -> index >= hash -> tabledim)
        {
                fprintf(stderr,"GetNextElement@hashADT.c - Invalid arguments to function GetNextElement\n");
                return 0;
        }
       
       
        while (hash -> index < hash -> tabledim && hash -> actual[hash -> index].status != OCCUPIED)
                hash -> index++;
       
        if (hash -> index >= hash -> tabledim)
                return 0;
       
        if (hash -> actual[hash -> index].status == OCCUPIED)
        {      
                if ( (*element = malloc(hash -> datasize)) == NULL )
                {
                        fprintf(stderr,"GetNextElement@hashADT.c - Not enough memory\n");
                        return 0;
                }
                else
                        memcpy(*element, hash -> actual[hash -> index++].data, hash -> datasize);
        }
       
        return aux;
}


int
SetHashBegin(hashADT hash)
{
        if ( hash == NULL )
                return 0;
       
        hash -> index = 0;
        hash -> actual = hash -> htable;
        return 1;
}

int
HDelete(hashADT hash, hashElementT data)
{
        int pos;
       
        if ( (pos = Lookup(hash, data)) == -1 )
        {
                fprintf(stderr,"HDelete@hashADT.c - Element not found, impossible to delete\n");
                return 0;
        }
       
        if ( hash -> htable[(pos + 1) % hash -> tabledim].status == PHYSICAL )
        {
                free(hash -> htable[pos].data);
                hash -> htable[pos].data = NULL;
                hash -> htable[pos].status = PHYSICAL;
        }
        else
                hash -> htable[pos].status = LOGICAL;
       
        return 1;
}

hashElementT
GetHElement(hashADT hash, int pos)
{
        hashElementT aux = NULL;
       
        if (  hash == NULL || hash -> htable == NULL || pos < 0 || pos > hash -> tabledim)
        {
                fprintf(stderr,"GetHElement@hashADT.c - invalid arguments\n");
                return NULL;
        }
       
        if (hash -> htable[pos].status == OCCUPIED)
        {
                if ( (aux = calloc(1, hash -> datasize)) == NULL )
                {
                        fprintf(stderr,"GetHElement@hashADT.c - not enough memory\n");

                }
               
                memcpy(aux, hash -> htable[pos].data, hash -> datasize);
                return aux;
        }
       

       
        return NULL;
}


int    
HTableIsEmpty(hashADT hash)
{
        if ( hash == NULL )
        {
                fprintf(stderr, "Invalid arguments to function HTableIsEmpty\n");
                return -1;
        }
       
       
       
        return (hash -> htable == NULL);
}

void
FreeHash(hashADT hash)
{
        int i;
       
        if (hash == NULL)
                return;
       
        for ( i = 0 ; i < hash -> tabledim ; i++ )
                if (hash -> htable[i].status != PHYSICAL )
                        free(hash -> htable[i].data);
       
        free(hash -> htable);
        free(hash);
        return;
}

int
SaveHashTable(hashADT table,char *path)
{
	FILE *fd;
	int i;

	if( (fd=fopen(path,"w+")) == NULL )
		return -1;
	for(i=0;i<table->tabledim;i++) {
		if( table->htable[i].status == OCCUPIED )
			table->hsave(fd,table->htable[i].data);
	}
	fclose(fd);
	return 1;
}

hashADT
LoadHashTable(char *path,int esize, hashCmp hcmp, hashKey hkey,hashSaveItem hsave,hashLoadItem hload)
{
	hashADT table = NewHash(esize,hcmp,hkey,hsave,hload);
	FILE *fd;
	hashElementT data;

	if((fd=fopen(path,"r"))==NULL) {		
		return NewHash(esize, hcmp, hkey, hsave,hload);
	}
	
	while( (data=hload(fd)) != NULL ) {
		HInsert(table,data);
	}
	fclose(fd);
	return table;
}

static int
Rehash(hashADT hash)
{
        int i, sizebkp;
        int iter = 0;
        htableT * newTable;
        htableT * backup;
       
        backup = hash -> htable;
        sizebkp = hash -> tabledim;
       
        /* se agranda con un numer impar del doble de dim */
       
        if ( (newTable = malloc((hash -> tabledim * 2 + 1) * sizeof(htableT))) == NULL )
                return 0;
       
        hash -> tabledim = sizebkp * 2 + 1;
       
        for (i = 0 ; i < hash -> tabledim ; i++)
                newTable[i].status = PHYSICAL;
       
        hash -> htable = newTable;
        hash -> qtty = 0; /* Reinicia la cantidad de elementos pues al re insertarlos incrementa en 1 */
       
        for (i = 0 ; i < sizebkp ; i++, iter = 0)
        {
                if (backup[i].status == OCCUPIED)
                {
                        if (!HInsert(hash, backup[i].data))
                        {
                                hash -> htable = backup;
                                hash -> tabledim = sizebkp;
                                free(newTable);
                                return 0;
                        }
                        free(backup[i].data);
                }
        }
       
        free(backup);
        return 1;
}

static int
InitTable(hashADT hash)
{
        if ( hash == NULL )
                return 0;
       
        if ( (hash -> htable = calloc(TABLE_INIT_SIZE, sizeof(htableT))) == NULL )
        {
                fprintf(stderr, "Not enough memory to initiate hash table\n");
                return 0;
        }
       
        return TABLE_INIT_SIZE;
}
