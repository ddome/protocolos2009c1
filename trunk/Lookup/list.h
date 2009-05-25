/*
 *  list.h
 *  MovieStoreLookup
 *
 *  Created by Damian Dome on 5/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __LIST_H__
#define  __LIST_H__

#include <string.h>
#include <stdio.h>

typedef char * object_key_t;

typedef struct listCDT* LIST;

LIST list_new(size_t key_size,size_t object_size);

int list_add(LIST list, object_key_t key, void *object);

int list_del(LIST list, object_key_t key);

int list_is_empty(LIST list);

LIST list_load(char *path,size_t key_size,size_t object_size);

LIST list_reload(LIST list, char *path,size_t key_size,size_t object_size);

void list_save(LIST list, char *path);

void * list_get(LIST list, object_key_t key);

void list_free(LIST list);

static int Order(object_key_t k1, object_key_t k2) {
	return strcmp(k1, k2);
}

static int Save(FILE *fd, object_key_t key,size_t key_size, void *object, size_t object_size) {
	
	int total;
	total = fwrite(key, key_size, 1, fd);
	total += fwrite(object, object_size, 1, fd);	
	return total;
}

static int Load(FILE *fd, object_key_t key,size_t key_size, void *object, size_t object_size) {
	
	int total;
	total = fread(key, key_size, 1, fd);
	total += fread(object, object_size, 1, fd);
	return total;
}

#endif


