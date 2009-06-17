#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include "listADT.h"

/* Nodo distinguido de la lista (header) */

struct listCDT
{
        struct nodoCDT * nodos;
        fn compara;
        fnfree freeElement;
        struct nodoCDT * actual;
};

/* Estructura de cada nodo de la lista */

typedef struct nodoCDT
{
        listElementT dato;
        struct nodoCDT * tail;
} nodoCDT;

static int
Error(char * msg)
{
    fprintf(stderr,"%s.\n",msg);
    return 0;
}

static nodoCDT *
ListTail(nodoCDT * sublist)
{
        if (sublist == NULL)
        {
                Error("Final node reached \n");
                return NULL;
        }

        return (sublist -> tail);
}

static void
FreeNodes(nodoCDT * nodo, fnfree freeElement)
{
        if (nodo == NULL)
                return;

        FreeNodes(nodo -> tail, freeElement);
        freeElement(nodo -> dato); /*libero el dato */
        free(nodo); /* libero el nodo */
        return;
}

listADT
Newlist(fn compara, fnfree freeElement)
{
        listADT auxi;

        if (compara == NULL || freeElement == NULL )
                return NULL;

        if ( (auxi = malloc(sizeof(struct listCDT))) == NULL)
        {
                Error("Not enought physical memory to allocate List \n");
                return NULL;
        }

        auxi -> nodos = NULL;
        auxi -> compara = compara;
        auxi -> freeElement = freeElement;
        auxi -> actual = NULL;

        return auxi;
}

int
Insert(listADT lista, listElementT dato)
{
        nodoCDT * anterior, * siguiente, * auxi;

        if (lista == NULL || dato == NULL)
        {
                Error("Invalid list or data \n");
                return 0;
        }
       
/* anterior y siguiente apuntan al primer nodo de la lista */

        anterior = siguiente = lista -> nodos;

        while (siguiente != NULL && lista -> compara(siguiente -> dato, dato) == -1)
        {
                anterior = siguiente;
        /* siguiente ahora es un puntero al siguiente nodoCDT */
                siguiente = ListTail(siguiente);
        }

/* Se chequea que el elemento no est� repetido */

        if (siguiente != NULL && lista -> compara(siguiente -> dato, dato) == 0)
        {
                Error("Element already exists \n");
                return 0;
        }

/* Se aloca la memoria para insertar el nuevo nodo */

        if ( (auxi = malloc(sizeof(struct nodoCDT))) == NULL )
        {
                Error("Not enought physical memory to allocate list node \n");
                return 0;
        }

/*
* el "tail" del nuevo nodo, apunta al nodo "siguiente", es decir el que quedo
* en la variable puntero nodoCDT "siguiente".
*/
        auxi -> tail = siguiente;

/* Guardo el dato
*/

	auxi->dato=dato;
/* si es el primer elemento a insertar, el header -> nodos es el nodo recien insertado.
*  sino, el "tail" del nodo anterior ahora apunta a el nodo insertado "auxi".
*/

        if (anterior == siguiente)
                lista -> nodos = auxi;
        else
                anterior -> tail = auxi;

        return 1;
}

int
Delete(listADT lista, listElementT dato)
{
        nodoCDT * anterior, * siguiente;

        anterior = siguiente = lista -> nodos;

        if (lista == NULL || dato == NULL)
        {
                Error("Invalid data type or NULL list \n");
                return 0;
        }

/* Mientras no encuentre el elemento recorre nodo x nodo la lista */

        while (siguiente != NULL && lista -> compara(siguiente -> dato, dato) == -1)
        {
                anterior = siguiente;
                siguiente = ListTail(siguiente);
        }

/* Si se llego al final de la lista, y el elemento no se encontro, no puede ser borrado */

        if (siguiente == NULL || lista -> compara(siguiente -> dato, dato) != 0)
        {
                Error("Element could not be deleted \n");
                return 0;
        }

/* Lo encontr�, por lo tanto habr� que borrarlo */

        if (anterior == siguiente)
                lista -> nodos = anterior -> tail; /*si es el primero, ahora el primer nodo del header sera el 2do */
        else
                anterior -> tail = siguiente -> tail; /* el nodo anterior apunta al siguiente del nodo "siguiente" */

/* Si en el header, el nodo "actual" apunta al que va a ser borrado, actualizo actual al nodo contiguo */

        if (siguiente == lista -> actual)
                lista -> actual = ListTail(lista -> actual);

        lista -> freeElement(siguiente -> dato); /* Borra con criterio deep copy */
        free(siguiente);
        return 1;
}

int
listIsEmpty(listADT lista)
{
        if (lista == NULL)
        {
                Error("Invalid list \n");
                exit(1);
        }
       
        return (lista -> nodos == NULL);
}


int
ElementBelongs(listADT lista, listElementT dato)
{
        nodoCDT * actual = lista -> nodos;
        int rta;
       
        if (lista == NULL || dato == NULL)
        {
                Error("Invalid data type or null list \n");
                return 0;
        }
               
/* Mientras no encuentre el elemento recorre nodo x nodo la lista */
               
        while (actual != NULL && lista -> compara(actual -> dato, dato) == -1)
                actual = ListTail(actual);
       
        if (actual == NULL || lista -> compara(actual -> dato, dato) != 0)
                rta = 0;
        else
                rta = 1;
       
        return rta;
}

void
SetBegin(listADT lista)
{
        if (lista == NULL || lista -> nodos == NULL)
                return;

        lista -> actual = lista -> nodos;
        return;
}

int
GetData(listADT lista, listElementT * element)
{
        nodoCDT * actual;
        int final = 1;
       
        if (listIsEmpty(lista) || lista == NULL)
        {
                Error("List is empty or invalid \n");
                exit(1);
        }
       
        actual = lista -> actual;
       
/* Si estoy en el �ltimo elemento se informa */
        if (actual == NULL)
        {      
                final = 0;
                return final;
        }
/* Se copia en el par�metro de respuesta en modo deep copy el dato del nodo actual */
       
	*element=actual->dato;
       
        lista -> actual = ListTail(actual);
        return final;
}

void
FreeList(listADT lista)
{
        if (lista == NULL)
        {
                Error("NULL list, unable to free \n");
                return;
        }
       
        SetBegin(lista); /* se itera desde el comienzo de la lista */
       
        FreeNodes(lista -> nodos, lista -> freeElement); /* libera todos los nodos */
        free(lista); /*libera la lista */
        return;
}

























