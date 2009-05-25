/*
 *  list.c
 *  MovieStoreLookup
 *
 *  Created by Damian Dome on 5/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "list.h"
#include <stdlib.h>

typedef struct node_t{
	object_key_t key;
	void *object;
	struct node_t *next;	
}node_t;

struct listCDT{
	node_t *first;
	size_t key_size;
	size_t object_size;
};

LIST 
list_new(size_t key_size,size_t object_size)
{
	LIST new = malloc(sizeof(struct listCDT));
	new->first = NULL;
	new->key_size = key_size;
	new->object_size = object_size;
	return new;
}

int 
list_add(LIST list, object_key_t key, void *object)
{
	node_t *node = malloc(sizeof(node_t));
	node_t *act;
	
	node->key = key;
	node->object = object;
	node->next = NULL;
	
	if( list->first == NULL ) {
		list->first = node;
		return 1;
	}
	
	act = list->first;
	while( act->next != NULL && Order(act->key,node->key) <= 0 ) {
		act = act->next;
	}
		
	node->next = act->next;
	act->next = node;
	return 1;
	
}

int 
list_is_empty(LIST list)
{
	if( list == NULL )
		return 1;
	if( list->first == NULL )
		return 1;
	return 0;
}

void *
list_get(LIST list, object_key_t key)
{
	node_t *act;
	
	if( list == NULL )
		return NULL;
	
	if( list_is_empty(list) )
		return NULL;
	
	act = list->first;
	while( act != NULL && Order(act->key,key) < 0 ) {
		act = act->next;
	}
	
	if( act == NULL )
		return NULL;
	
	if( Order(act->key,key) > 0 )
		return NULL;
	
	return act->object;	
}

LIST
list_load(char *path,size_t key_size,size_t object_size)
{
	FILE *fd;
	LIST list = list_new(key_size,object_size);
	
	char *aux_key;
	void *aux_object;
	
	char *key = malloc(key_size);
	void *object = malloc(object_size);
	
	if( (fd=fopen(path,"rb")) == NULL )
		return NULL;
	
	while( Load(fd,key,key_size,object,object_size) > 0 ) {
		aux_key = malloc(key_size);
		aux_object = malloc(object_size);
		memmove(aux_key, key, key_size );
		memmove(aux_object, object, object_size);
		list_add(list, aux_key, aux_object);
	}
	free(key);
	free(object);
	fclose(fd);
	
	return list;
}

LIST 
list_reload(LIST list, char *path,size_t key_size,size_t object_size)
{
	free(list);
	return list_load(path, key_size, object_size);
}

void
list_save(LIST list, char *path)
{
	FILE *fd;
	node_t *act;
	
	if( (fd=fopen(path,"wb+")) == NULL )
		return;
	
	if( list_is_empty(list) )
		return;
	
	act = list->first;
	
	while( act != NULL ) {
		Save(fd,act->key,list->key_size,act->object,list->object_size);
		act = act->next;
	}
	
	fclose(fd);
	
}

void 
list_free(LIST list)
{
	node_t *act,*next;
	if( list == NULL )
		return;
	if( !list_is_empty(list) ){
		act = list->first;
		while( act != NULL ){
			next = act->next;
			free(act);
			act = next;
		}
	}
	
	free(list);
}
