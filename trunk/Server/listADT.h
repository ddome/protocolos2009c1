#ifndef _LIST_H_
#define _LIST_H_

/* Definicin tipo de elementos de la lista */

typedef void * listElementT;

/* Definicines de puntero a funcin de comparacin */

typedef int (*fn) (listElementT, listElementT);
typedef void (*fnfree) (listElementT);

/* Definicion del ADT lista */

typedef struct listCDT * listADT;

/* Operaciones del ADT*/

/*
* Funcion:              Newlist
* Uso:                  if ((lista = Newlist( funcCompara )) == NULL); ...
*
*
* Descripcin:           Inicializa la lista y setea la funcin de comparacin "funcCompara"
*                       para sus elementos y el tamao de los elementos que maneja la lista.
*                       Tambin setea la funcin de copia de elementos y de liberacion de elementos
*                       que es responsabilidad del usuario del ADT la correctitud de las mismas.
*                       "funcCompara" compara dos elementos de tipo listElementT y debe
*                       devolver un nmero < 0, == 0, > 0 si el primer elemento es menor,
*                       igual o mayor al segundo respectivamente.
*                       Usar antes y por nica vez previo al resto de las operaciones del ADT.
*              
* Precondicin:          Funcin de comparacin, copiado y borrado no nulas. tamao elemento > 0.
* Postcondicin: 	Lista creada.
*/

listADT Newlist(fn compara, fnfree freeElement);

/*
* Funcion               InsertADT
* Uso:                  int numero = 4;
*                       InsertADT(lista, &numero);

* Descripcin:           Inserta un elemento en la lista ordenada, mediante el uso
*                       de la funcin de comparacin "funcCompara". Se debe enviar
*                       como parmetro la DIRECCION del elemento a insertar.
*                       Si el elemento est repetido, lo ignora, si hay problemas
*                       con la memoria, no inserta el elemento y retorna.
*
* Precondicin:          Lista vlida previamente creada, puntero al elemento vlido.
* Postcondicin: 	Lista con el nuevo elemento insertado (de no estar repetido).
*/

int Insert(listADT lista, listElementT dato);

/*
* Funcion               DeleteADT
* Uso:                  int numero = 4;
*                       if (DeleteADT(lista, &numero, freeElement) ...);
*
* Descripcin:           Elimina el elemento de la lista ordenada, usando la funcin de
*                       comparacin "funcCompara" en cada elemento de la lista hasta
*                       encontrar el que tiene que ser borrado. Si el elemento no exita
*                       devuelve 0, caso contrario lo borr y devuelve 1;
*
* Precondicin:          Lista vlida existente, dato y funcin de liberacin no nulos.
* Postcondicin: Lista sin el elemento(y nodo :) ).
*/

int Delete(listADT lista, listElementT dato);

/*
* Funcion               ListIsEmpty
* Uso:                  if (listIsEmpty(lista, freeElement) ...)

* Descripcin:           Devuelve un 1 si la lista est vaca o  0
*                       en caso contrario.
*
* Precondicin:          lista vlida previamente creada. Funcin de liberacin no nula.
* Postcondicin: 	lista invariante.
*/

int listIsEmpty(listADT lista);

/*
* Funcion               ElementBelongsADT
* Uso:                  int numero = 14;
*                       if (ElementBelongsADT(lista, &numero)...)
*
* Descripcin:           Devuelve 1 si el elemento pertenece a la lista o 0
*                       en caso contrario.
*
* Precondicin:          lista previamente creada.
* Postcondicin: 	lista invariante.
*/

int ElementBelongs(listADT lista, listElementT dato);

/*
* Funcion:              SetBeginADT
* Uso:                  SetBeginADT(lista);
*
* Descripcin:           Permite recorrer la lista desde el principio, se usa
*                       conjuntamente con GetDataADT.
*
* Precondicin:          lista previamente creada.
* Postcondicin: 	Proximo nodo que se accede con GetDataADT -> el primero.
*/

void SetBegin(listADT lista);

/*
* Funcion:              GetDataADT
* Uso:                  listADT lista
*
*                       int dato;
*
*                       SetBeginADT(lista);
*                       final = GetDataADT(lista, &dato);
*
* Descripcin:           Sirve para iterar sobre los elementos de la lista. Devuelve 0
*                       cuando se invoca esta funcin y se est sobre el ltimo nodo de la
*                       lista o en caso contrario devuelve un 1 y as sucesivamente
*                       sobre cada invocacin.
*                       El segundo parmetro de GetDataADT, debe ser un  puntero de cualquier
*                       tipo con memoria previamente reservada (en stack o heap)
*                       por el usuario del tamao del elemento donde se dejar el valor del nodo.
*
* Precondicin:          lista no vaca y vlida
* Postcondicin: 	lista invariante.
*/

int GetData(listADT lista, listElementT * element);

/*
* Funcion:              FreeListADT
* Uso:                  FreeListADT(lista, freeElement);
*
* Descripcin:           Libera los recursos alocados para la lista.
*
* Precondicin           lista previamente creada no nula. Funcin de liberacin no nula
* Postcondicin: lista destruda.
*/

void FreeList(listADT lista);

#endif




















