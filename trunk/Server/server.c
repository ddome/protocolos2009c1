/*
 *  server.c
 *  MovieStoreServer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "../Common/genlib.h"
#include "client_ldap.h"
#include "server.h"
#include "../Common/TCPLib.h"
#include "hashADT.h"
#include "../Common/des/include/encrypt.h"
#include "../Common/fileHandler.h"

/* Variable global que guarda la coneccion con el servidor LDAP */
LDAP *ld;

/* Puerto de conexion del servidor */
int passive_s;

/* Informacion de los usuarios online */
hashADT users_online;

/* Static functions */

static boolean UserCanAcces(char *user,char *passwd);

static status UserDelete(char *user,char *passwd);

static status SendMovie(char *path,char *ip,char *port);

/* GetPack(data,&pack) */

static u_size GetHeaderPack(void *data, header_t *header);

static u_size GetLoginPack(void *data, login_t *log);

static u_size GetNewUserPack(void *data, client_t *client);

static u_size GetDownloadStartOK(void *data, download_start_t * download_start);

static u_size GetRequest(void *data, request_t * request);

static u_size GetDownloadHeaderData( download_header_t pack, void **data_ptr);

static u_size GetDownloadData( download_t pack, void **data_ptr);

status 
InitServer(void)
{
	int ret;
	
	/* Iniciar el servidor ldap */
	
	if( (ld=InitLdap()) == NULL ) {
		return FATAL_ERROR;
	}
		
	/* Iniciar TCP */
	
	if( (passive_s=prepareTCP(HOST_DOWNLOAD,PORT_SERVER,prepareServer)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",passive_s);
		return FATAL_ERROR;
	}	
	if( (ret=listenTCP(passive_s,10)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",ret);
		return FATAL_ERROR;
	}
		
	return OK;
}	
	

status 
StartServer(void)
{
	int ssock;
	void * packet;
	fd_set rfds;
	fd_set afds;	
	int fd, nfds;
	
	/* Mantengo la informacion de los usuarios online */
	users_online = NewHash(150, UsersComp, UsersHash);
	
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(passive_s,&afds);
	
	while(1) {
		
		memcpy(&rfds, &afds, sizeof(rfds));
		
		if( select(nfds, &rfds, NULL, NULL,NULL) < 0 ) {
			fprintf(stderr, "select: %s\n", strerror(errno));
			return FATAL_ERROR;
		}
		
		/* Si es una nueva conexion la agrego */
		if (FD_ISSET(passive_s, &rfds)) {
						
			if( (ssock=acceptTCP(passive_s)) <= 0 ) {
				printf("Fallo acceptTCP() - retCode=(%d)\n",ssock);
				return FATAL_ERROR;
			}
			
			FD_SET(ssock, &afds);
		}
		
		/* Atiendo cada pedido */
		for(fd=0; fd<nfds; ++fd) {
			if (fd != passive_s && FD_ISSET(fd, &rfds)) {
								
				packet=receiveTCP(fd);
				/* Proceso el packet */
				if( Session(packet,fd) != FATAL_ERROR ) {
					close(fd); // Tengo que cerrar la conexion?
					FD_CLR(fd, &afds);
				}
				else {
					return FATAL_ERROR;
				}
					
			}
		}
	}	
	closeTCP(passive_s);
	
	return OK;
}

void
EndServer(void)
{
	EndLdap(ld);
}


/* Atencion de pedidos */


status
Session(void *packet,int socket)
{
	header_t header;	
	login_t log;
	client_t client;
	request_t req;
	download_start_t start;
	u_size header_size;
	
	
	/* Levanto el header del paquete */
	//memmove(&header, packet, sizeof(header));
	
	header_size = GetHeaderPack(packet,&header);
	
	switch (header.opCode) {
		case __USER_LOGIN__:
			/* Logueo al ususario */
			GetLoginPack(packet+header_size,&log);
			fprintf(stderr,"Llego un pedido de --login-- de user:%s passwd:%s\n",log.user,log.passwd);
			return UserLogin(log,socket);
			break;
		case __NEW_PASSWD__:
			/* Cambio de clave */
			memmove(&log, packet+header_size, sizeof(login_t) );
			fprintf(stderr,"Llego un pedido de --password-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserNewPasswd(log,socket,header.user,header.passwd);
			break;
		case __REG_USER__:
			/* Registro de un nuevo usuario */
			GetNewUserPack(packet+header_size,&client);
			fprintf(stderr,"Llego un pedido de --new-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserRegister(client,socket);
			break;
		case __DOWNLOAD__:
			/* Descargar pelicula */
			fprintf(stderr,"Llego un pedido de --download-- de user:%s passwd:%s\n",header.user,header.passwd);
			GetRequest(packet+header_size,&req);
			return UserDownload(req,socket, header.user, header.passwd);
			break;
		case __DOWNLOAD_START_OK__:
			/* Descargar pelicula */
			fprintf(stderr,"Llego un pedido de --startdownload-- de user:%s passwd:%s\n",header.user,header.passwd);
			GetDownloadStartOK(packet+header_size,&start);
			return UserStartDownload(start,socket,header.user,header.passwd);
			break;	
		case __LOG_OUT__:
			/* Desconectar usuario */
			fprintf(stderr,"Llego un pedido de --logout-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserLogout(socket, header.user, header.passwd);
			break;		
		default:
			fprintf(stderr, "No se reconocio el op_code:%d\n",header.opCode);
			return ERROR;
			break;
	}
	
}

status
UserLogin(login_t log,int socket)
{
	int ret;	
	ack_t to_ack;
	char *user;
	char *passwd;
	login_t *log_ptr;
			
	user = CopyString(log.user);
	passwd = CopyString(log.passwd);
		
	ret = __LOGIN_OK__;
		
	if( !UserExist(ld, user) ) 
		ret = __USER_ERROR__;	
	else if( !PasswdIsValid(ld, user, passwd) )
		ret = __PASSWD_ERROR__;
	
	if( ret == __LOGIN_OK__ ) {
		/* Si ya esta logueado no lo vuelve a insertar */
		if(  Lookup(users_online, &log) == -1 ) {
			if( (log_ptr = malloc(sizeof(login_t))) == NULL )
				return FATAL_ERROR;
			*log_ptr = log;
			if( HInsert(users_online, log_ptr) == 0 )
				return FATAL_ERROR;
		}
		else
			ret = __USER_IS_LOG__;
	}
	/* Mando al respuesta */
	to_ack.ret_code = ret;
	sendTCP(socket, &to_ack, sizeof(ack_t));
	
	return OK;
}

status 
UserNewPasswd(login_t log,int socket, char *user,char *passwd)
{
	char *aux_user;
	char *aux_passwd;
	int ret = __CHANGE_OK__;
	ack_t ack;
	login_t *log_ptr;
	char *des_passwd;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	
	/* Si pasa los controles, cambio su clave */
	if( ret == __CHANGE_OK__ ) {		
		/* Desencripto la nueva clave */
		des_passwd = malloc(strlen(log.passwd)+1);
		des_decipher(log.passwd, des_passwd, passwd);
		fprintf(stderr, "--%s--\n",des_passwd);
		strcpy(log.passwd,des_passwd);
		aux_user = CopyString(log.user);
		aux_passwd = CopyString(log.passwd);
		/* Llamado a la funcion que cambia la password en el servidor ldap */
		ChangePasswd(ld, aux_user, aux_passwd);
		free(aux_user);
		free(aux_passwd);
		free(des_passwd);
		/* Borro e inserto el usuario con la nueva password en login_users */
		UserDelete(user, passwd);
		fprintf(stderr, "--%s--\n",log.passwd);
		if( (log_ptr = malloc(sizeof(login_t))) == NULL )
			return FATAL_ERROR;
		*log_ptr = log;
		if( HInsert(users_online, log_ptr) == 0 )
			return FATAL_ERROR;		
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));
		
	return OK;
}

status
UserRegister(client_t client,int socket)
{
	int ret = __REG_OK__;
	ack_t ack;
	
	fprintf(stderr, "%s %s %s %s\n", client.user,client.passwd,client.mail,client.desc);
	
	
	/* Llamo a la funcion que pregunta si un nombre de usuario ya existe en la base ldap */
	if( UserExist(ld, client.user) )
		ret = __REG_USER_ERROR__;

	if( ret == __REG_OK__ ) {		
		/* Llamo a la funcion que agrega un cliente a la base ldap */
		if( ClientAdd(ld,client) == ERROR )
			ret = __REG_ERROR__;
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));
	
	return OK;
}

status
UserDownload(request_t req,int socket,char *user,char *passwd)
{
	int ret = __DOWNLOAD_START__;
	download_header_t ack;
	file_info_t file_info;
	u_size size;
	void *data;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	/* Busco la informacion del archivo, verifico permisos y nivel de descarga */
	if( ret == __DOWNLOAD_START__ ) {
		/*if( GetMovieInfo(req.ticket,&file_info) == -1 ) {
			ret = __DOWNLOAD_ERROR__;
		}
		strcpy(ack.title,file_info.movie.name);
		ack.size = file_info.movie.size;
		 */
		strcpy(ack.title,"SpiderMan");
		ack.size = GetFileSize("test");
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	size = GetDownloadHeaderData(ack,&data);
	sendTCP(socket, &ack, sizeof(download_header_t));

	return OK;
}

status
UserStartDownload(download_start_t start,int socket, char *user, char *passwd)
{		
	file_info_t file_info;
	
	
	fprintf(stderr,"Antes\n");
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		return OK;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		return OK;
	}
	/* Busco la informacion del archivo, verifico permisos y nivel de descarga */
	/*if( GetMovieInfo(req.ticket,&file_info) == -1 ) {
		return OK;
	}*/
	
	fprintf(stderr,"%s %s\n",start.ip,start.port);
	
	switch( fork() ) {
		case 0:
			/*SendMovie(file_info.path,start.ip,start.port);*/
			
			/* Espero a que se establezca la conexion */
			sleep(2);
			fprintf(stderr,"Empiezoooooo\n");
			if( SendMovie("test",start.ip,start.port) != OK )
				exit(EXIT_FAILURE);
			else
				exit(EXIT_SUCCESS);
			break;
		case -1:
			return ERROR;
			break;
		default:
			return OK;
			break;
	}
}

status
UserLogout(int socket, char *user, char *passwd)
{
	int ret = __LOG_OUT_OK__;
	ack_t ack;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	/* Borro al usuario de la lista de usuarios conectados */
	if( ret == __LOG_OUT_OK__ ) { 
		if( UserDelete(user,passwd) == ERROR )
			ret = __LOG_OUT_ERROR__;
	}       
	/* Mando la respuesta */
	ack.ret_code = ret;
	sendTCP(socket, &ack, sizeof(ack_t));   
	
	return OK;
}


/* Funciones del manejo de la tabla de hashing */

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

/* Static Functions */

/* Funcion que controla la identificacion de los usuarios */
static boolean 
UserCanAcces(char *user,char *passwd)
{
	int pos;
	login_t id;
	login_t *aut;
	strcpy(id.user, user);
	strcpy(id.passwd, passwd);
	
	/* Control de seguridad del usuario */
	if( (pos=Lookup(users_online,&id)) == -1 ) {
		return FALSE;
	}	
	/* Control de seguridad del password */	
	aut = GetHElement(users_online,pos);
	if( strcmp(aut->passwd,passwd ) != 0 ) {
		return FALSE;
	}	
	
	return TRUE;
}

static status
UserDelete(char *user,char *passwd)
{
	int pos;
	login_t id;
	login_t *aut;
	strcpy(id.user, user);
	strcpy(id.passwd, passwd);
	/* Busco el elemento previamente para poder liberarlo */
	if( (pos=Lookup(users_online,&id)) == -1 ) {
		return ERROR;
	}	
	aut = GetHElement(users_online,pos);
	/* Borro el elemento de la lista de usuarios online */
	HDelete(users_online, &id);
	/* Libero el puntero usado */
	free(aut);
	return OK;
}

static status
SendMovie(char *path,char *ip,char *port)
{
	download_t header;
	void *data;
	void *header_data;
	u_size total_packets;
	u_size bytes_read;
	int socket;
	FILE *fd;
	void *to_send;
	u_size header_size;
	int num;
	
	/* Me conecto al cliente */
	if( (socket=connectTCP(ip,port)) < 0 ){
		return ERROR;
	}
	total_packets = SplitFile(path,_FILE_SIZE_);
	/* Mando los paquetes */
	int i;
	header.total_packets = total_packets;
	fd = fopen(path,"rb");
	for(i=0;i<total_packets;i++) {
		bytes_read = GetFileData(fd,_FILE_SIZE_,i,&data);
		header.size = bytes_read;
		header.n_packet = i;
		header_size = GetDownloadData(header, &header_data);
		to_send = malloc(header_size+bytes_read);
		memmove(to_send, header_data, header_size);
		memmove(to_send+header_size, data, bytes_read);;
		num = sendTCP(socket, to_send,bytes_read+header_size) < 0;
			
		fprintf(stderr,"TCP %d\n", num);
		
		sleep(1);
		
		free(data);
		free(header_data);
		free(to_send);
	}
	
	close(socket);
	fclose(fd);
	fprintf(stderr,"Termine de transmitir\n");
	return OK;
}

/* struct pack = GetPack(data) */

static u_size
GetHeaderPack(void *data, header_t *header)
{	
	u_size pos;
	
	pos=0;
	memmove(&(header->opCode), data, sizeof(unsigned long int));
	pos+=sizeof(unsigned long int);
	memmove(&(header->total_objects), data+pos, sizeof(unsigned long int));
	pos+=sizeof(unsigned long int);	
	memmove(header->user, data+pos, MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(header->passwd, data+pos, MAX_USER_PASS);	
	pos+=MAX_USER_PASS;
	
	return pos;
}

static u_size
GetLoginPack(void *data, login_t *log)
{	
	u_size pos;
	
	pos = 0;
	memmove(log->user, data, MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(log->passwd, data + MAX_USER_LEN, MAX_USER_PASS);
	pos+=MAX_USER_PASS;

	return pos;
}

static u_size
GetDownloadStartOK(void *data, download_start_t * download_start)
{
    u_size pos;
    pos=0;
    memmove(download_start->ip,data,50);
    pos+=50;
    memmove(download_start->port,data+pos,10);
    pos+=10;
    
    return pos;
}

static u_size
GetRequest(void *data, request_t * request)
{
    u_size pos;
    pos=0;
    memmove(request->ticket,data,20);
    pos+=20;
    
    return pos;
}

static u_size
GetNewUserPack(void *data, client_t *client)
{	
	u_size pos;
	
	pos = 0;
	memmove(client->user, data, MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(client->passwd, data + pos , MAX_USER_PASS);
	pos+=MAX_USER_PASS;
	memmove(client->mail, data + pos , MAX_USER_MAIL);
	pos+=MAX_USER_MAIL;
	memmove(client->desc, data + pos , MAX_USER_DESC);
	pos+=MAX_USER_DESC;
	memmove(&(client->level), data + pos , sizeof(unsigned char));
	pos+=sizeof(unsigned char);
	
	return pos;
}


static u_size
GetDownloadHeaderData( download_header_t pack, void **data_ptr)
{
	void *data;
	u_size size;
	u_size pos;
	
	size = sizeof(int) + MAX_MOVIE_LEN + MAX_PATH_LEN 
			+ sizeof(unsigned long) + sizeof(u_size);
	
	if( (data=malloc(size)) == NULL )
		return -1;
	
	pos=0;
	memmove(data, &(pack.ret_code), sizeof(int));
	pos+=sizeof(int);
	memmove(data+pos, pack.title, MAX_MOVIE_LEN);
	pos+=MAX_MOVIE_LEN;
	memmove(data+pos, pack.path, MAX_PATH_LEN);
	pos+=MAX_PATH_LEN;
	memmove(data+pos, &(pack.total_packets), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, &(pack.size), sizeof(u_size));
	pos+=sizeof(u_size);
	
	*data_ptr = data;
	return size;
	
}

static u_size
GetDownloadData( download_t pack, void **data_ptr)
{
	void *data;
	u_size size;
	u_size pos;
	
	size = sizeof(unsigned long) * 2 + sizeof(u_size);
	
	if( (data=malloc(size)) == NULL )
		return -1;
	
	pos=0;
	memmove(data, &(pack.n_packet), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, &(pack.total_packets), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, &(pack.size), sizeof(u_size));
	pos+=sizeof(u_size);
	
	*data_ptr = data;
	return size;
	
}


