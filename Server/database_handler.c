/*
 *  database_handler.c
 *  MovieStoreServer
 *
 */

#include "database_handler.h"
#include "../Common/fileHandler.h"


/*******************************************************************************************************/
/*                           Funciones de manejo de la tabla de hashing                                */
/*******************************************************************************************************/


#define MAX_LINE 4646

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

/* Lista de peliculas */

static movie_t * BuildMovie(char * line,char ** pathNameRet);


static char *
ReadLine( FILE * inputFile )
{
    char line[MAX_LINE+ 1];
    char * resp;
    int len;
	
    if( fgets( line, MAX_LINE, inputFile ) ==NULL )
		return NULL;
    if( ((len = strlen( line )) == 0) || ((len = strlen( line )) == 1 ) )
        return NULL;
    line[len-1] = '\n';
    line[len] = '\0';
	
    resp = malloc( (len + 1) * sizeof(char) );
    strncpy(resp, line, len + 1);
	
    return (resp);
}

int
InitDB(dbADT db,char * pathName,hashADT files_info)
{
    FILE * file;
    char * line;
    char * pathNameMovie;
    movie_t * movie;
	
	
    if( (file=fopen(pathName,"r"))==NULL )
		return ERROR;
    
    while( (line=ReadLine(file))!=NULL /*&& line[0]!='\n'*/)
    {
		movie=BuildMovie(line,&pathNameMovie);
		if(movie==NULL)
		{
			free(line);
			return ERROR;
		}
		free(line);		
		InsertMovie(db,movie,pathNameMovie);
		/* Guardo una copia del path en una tabla de hash
		 * para su posterior busqueda ante una descarga */
		file_info_t * file_info=malloc(sizeof(file_info_t));
		strcpy(file_info->name,movie->name);
		strcpy(file_info->path, pathNameMovie);
		strcpy(file_info->MD5, movie->MD5);
		file_info->value = movie->value;
		HInsert(files_info, file_info);
	
		free(movie);
		free(pathNameMovie);
    }
    fclose(file);
    return OK;
}

static movie_t *
BuildMovie(char * line,char ** pathNameRet)
{
    movie_t * resp;
    char * aux;
    char pathName[MAX_PATH_LEN];
    char * md5;
    
    if( (resp=malloc(sizeof(movie_t)))==NULL )
		return NULL;
    
    aux=strtok(line,";");
	
    if(aux!=NULL)
		strcpy(pathName,aux);
    else
    {
		free(resp);
		return NULL;
    }
    
    if( (*pathNameRet=malloc( sizeof(char) * (strlen(pathName)+1) ))==NULL )
    {
		free(resp);
		return NULL;
    }
    strcpy(*pathNameRet,pathName);
    /*Ve si el archivo asociado al pathName existe*/
    if( !FileExists(pathName) ) {
	    free(resp);
	    return NULL;
    }
    
    /*Nombre de la pelicula*/
    aux=strtok(NULL,";");
    if(aux!=NULL)
		strcpy(resp->name,aux);
    else
    {
		free(resp);
		return NULL;
    }
	
    /*Genero de la pelicula*/
    aux=strtok(NULL,";");
    if(aux!=NULL)
		strcpy(resp->gen,aux);
    else
    {
		free(resp);
		return NULL;
    }
	
    /*Trama de la pelicula*/
    aux=strtok(NULL,";");
    if(aux!=NULL)
		strcpy(resp->plot,aux);
    else
    {
		free(resp);
		return NULL;
    }
	
    /*Duracion de la pelicula*/
    aux=strtok(NULL,";");
    if(aux!=NULL)
		resp->duration=(long)atoi(aux);
    else
    {
		free(resp);
		return NULL;
    }
    
    /*Tamaño de la pelicula*/
    resp->size = GetFileSize(pathName);
    
    /*Costo de la pelicula*/
    aux=strtok(NULL,";");
    if(aux!=NULL){
		int status=sscanf(aux,"%f",&resp->value);
		if( status == 0 ){
			free(resp);
			return NULL;
		}			
	}
    else
    {
		free(resp);
		return NULL;
    }
	
    /*md5=getMD5(pathName);
	 strcpy(resp->MD5,md5);
	 free(md5);*/
    strcpy(resp->MD5,"666");
    
    aux=strtok(NULL,";");
    if(aux!=NULL)
    {
		free(resp);
		return NULL;
    }
    
    return resp;
}
	
/* Buffer temporal de lookup */
int
ServersComp( void * v1, void *v2 )
{
	payment_server_t *t1,*t2;
	
	t1 = v1;
	t2 = v2;
	
	return strcmp(t1->name, t2->name);
}

int
ServerHash( void *v, int size )
{
	int i;
	payment_server_t *t = v;
	int num = 0;
	
	i=0;
	while (t->name[i] != '\0' ) {
		num += (t->name)[i];
		i++;
	}	
	return  num % size;
}

int
ServerSave(FILE *fd,void *data)
{	
	payment_server_t *f =data;
	fprintf(fd,"%s;%s;%s;%s;%ld\n",f->name,f->host,f->port,f->key,f->TTL);
	
	return 0;
}

void *
ServerLoad(FILE *fd)
{	
	payment_server_t *f = malloc(sizeof(payment_server_t));
	char line[500];
	char *token;
	
	if( fgets(line,500,fd) == NULL )
		return NULL;
	
	token = strtok (line,";");
	if( token == NULL )
		return NULL;
	strcpy(f->name,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->host,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->port,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;
	strcpy(f->key,token);
	
	token = strtok (NULL,";");
	if( token == NULL )
		return NULL;	
	int status=sscanf(token,"%ld",&f->TTL);
	if( status == 0 ){
		return NULL;
	}
	
	return (void*)f;
}

static payment_server_t *
GetServer(hashADT table, char *server_name)
{
	payment_server_t aux;
	int pos;
	
	strcpy(aux.name, server_name);
	if( (pos=Lookup(table, &aux)) == -1 )
		return NULL;
	
	return GetHElement(table, pos);
}



