/*
 *  database_handler.c
 *  MovieStoreServer
 *
 */

#include "database_handler.h"


/*******************************************************************************************************/
/*                           Funciones de manejo de la tabla de hashing                                */
/*******************************************************************************************************/

/* Usuarios conectados */

int
UsersComp( void *v1, void *v2 )
{
	login_t *c1=(login_t *)v1;
	login_t *c2=(login_t *)v2;
	
	return strcmp( c1->user, c2->user );
}

int
UsersHash( void *v1, int size )
{
	int hash = 0;
	login_t *c1 = (login_t *)v1;
	int len  = strlen(c1->user);
	int i;
	
	for(i=0;i<len;i++){
		hash += c1->user[i];
	}
	
	return hash % size;
}


/* Tickets generados */

int
TicketInfoComp( void * v1, void *v2 )
{
	ticket_info_t *t1,*t2;
	
	t1 = v1;
	t2 = v2;
	
	return strcmp(t1->ticket, t2->ticket);
}

int
TicketHash( void *v, int size )
{
	int i;
	ticket_info_t *t = v;
	int num = 0;
	
	i=0;
	while((t->ticket)[i] != '\0' ) {
		num += (t->ticket)[i];
		i++;
	}	
	return  num % size;
}

int
TicketSave(FILE *fd,void *data)
{	
	if( fwrite(data,sizeof(ticket_info_t),1,fd) < 1)
		return -1;
	
	return 0;
}

void *
TicketLoad(FILE *fd)
{	
	void *data = malloc(sizeof(ticket_info_t));
	if( fread(data,sizeof(ticket_info_t),1,fd) < 1 )
		return NULL;
	
	return data;
}

/* Peliculas disponibles */

int
FileInfoComp( void * v1, void *v2 )
{
	file_info_t *t1,*t2;
	
	t1 = v1;
	t2 = v2;
	
	return strcmp(t1->name, t2->name);
}

int
FileInfoHash( void *v, int size )
{
	int i;
	file_info_t *t = v;
	int num = 0;
	
	i=0;
	while (t->name[i] != '\0' ) {
		num += (t->name)[i];
		i++;
	}	
	return  num % size;
}

int
FileInfoSave(FILE *fd,void *data)
{	
	file_info_t *f =data;
	fprintf(fd,"%s;%s\n",f->name,f->path);
	
	return 0;
}

void *
FileInfoLoad(FILE *fd)
{	
	file_info_t *f = malloc(sizeof(file_info_t));
	int c;
	char line[50];
	char *token;

	if( fgets(line,50,fd) == NULL )
		return NULL;

	token = strtok (line,";");
	if( token == NULL )
		return NULL;
	strcpy(f->name,token);

	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->path,token);
	
	return (void*)f;
}





