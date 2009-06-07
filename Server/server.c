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
#include "../Common/UDPLib.h"
#include "hashADT.h"
#include "counter.h"
#include "../Common/des/include/encrypt.h"
#include "../Common/fileHandler.h"
#include "database_handler.h"

#define PAY_OK 1

/* Variable global que guarda la conexion con el servidor LDAP */
LDAP *ld;

/* Puerto de conexion del servidor */
int passive_s;

/* Informacion de los usuarios online */
hashADT users_online;

/* Informacion de los tickets generados */
hashADT tickets_generated;

/* Informacion de los archivos disponibles */
hashADT file_paths;

/* Informacion de los tickets generados */
COUNTER tickets_counter;


/************************************************************/
/*                    Static functions                      */
/************************************************************/

static boolean UserCanAcces(char *user,char *passwd);

static status UserDelete(char *user,char *passwd);

static status SendMovie(char *path,char *ip,char *port);

static payment_server_t SendPaymentServerLocationRequest( char *name );

static int PayMovie(char *pay_name,char *pay_user,char *pay_passwd);

static char * MakeTicket(char *user,char *movie_name);

static ticket_info_t * GetTicketInfo(char *ticket);

static file_info_t * GetFileInfo(char *name);


/************************************************************/
/*                      GetPack(data)                       */
/************************************************************/

static u_size GetPaymentLocationPack(void * ack_data,
				      payment_server_t * ack);

static u_size GetHeaderPack(void *data, header_t *header);

static u_size GetLoginPack(void *data, login_t *log);

static u_size GetNewUserPack(void *data, client_t *client);

static u_size GetDownloadStartOK(void *data, download_start_t * download_start);

static u_size GetRequest(void *data, request_t * request);

/************************************************************/
/*                   GetData(pack)                          */
/************************************************************/

static u_size GetPaymentRequestData(server_request_t req,
                                  void ** req_data);

static u_size GetDownloadHeaderData( download_header_t pack, 
                                    void **data_ptr);

static u_size GetDownloadData( download_t pack,
                                void **data_ptr);

static u_size GetBuyMoviePack(void *data,buy_movie_request_t *buy);

static u_size GetBuyTicketData( buy_movie_ticket_t pack, void **data_ptr);


/************************************************************/


status 
InitServer(void)
{
	int ret;
	
	/* Iniciar el servidor ldap */
	if( (ld=InitLdap()) == NULL ) {
		return FATAL_ERROR;
	}
		
	/* Iniciar TCP */
	if( (passive_s=prepareTCP(HOST_SERVER,PORT_SERVER,prepareServer)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",passive_s);
		return FATAL_ERROR;
	}	
	if( (ret=listenTCP(passive_s,10)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",ret);
		return FATAL_ERROR;
	}
	
	/* Iniciacion de tablas de datos */
	
	/* Usuarios online */
	users_online = NewHash(150, UsersComp, UsersHash,NULL,NULL);	
	
	/* Tickets generados asociados a una descarga */
	tickets_generated = LoadHashTable(TICKETS_DATA_PATH, 150, TicketInfoComp, TicketHash, TicketSave, TicketLoad);
	
	/* Ubicacion de las peliculas dentro del file system */
	file_paths = LoadHashTable(FILES_DATA_PATH, 150, FileInfoComp, FileInfoHash, FileInfoSave, FileInfoLoad);
	
	/* Tickets disponibles */
	tickets_counter = LoadCounter(TICKETS_FREE_PATH);
		
	return OK;
}

status 
StartServer(void)
{
	int ssock;
	void * data;
	fd_set rfds;
	fd_set afds;	
	int fd, nfds;
	
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
								
				data=receiveTCP(fd);
				/* Proceso el paquete */
				if( Session(data,fd) != FATAL_ERROR ) {
					close(fd); // Tengo que cerrar la conexion?
					FD_CLR(fd, &afds);
					free(data);
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

/*******************************************************************************************************/
/*                                       Atencion de pedidos                                           */
/*******************************************************************************************************/

status
Session(void *data,int socket)
{
	header_t header;	
	login_t log;
	client_t client;
	request_t req;
	download_start_t start;
	u_size header_size;
	buy_movie_request_t buy;
	
	
	/* Levanto el header del paquete */	
	header_size = GetHeaderPack(data,&header);
	
	switch (header.opCode) {
		case __USER_LOGIN__:
			/* Logueo al ususario */
			GetLoginPack(data+header_size,&log);
			fprintf(stderr,"Llego un pedido de --login-- de user:%s passwd:%s\n",log.user,log.passwd);
			return UserLogin(log,socket);
			break;
		case __NEW_PASSWD__:
			/* Cambio de clave */
			memmove(&log, data+header_size, sizeof(login_t) );
			fprintf(stderr,"Llego un pedido de --password-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserNewPasswd(log,socket,header.user,header.passwd);
			break;
		case __REG_USER__:
			/* Registro de un nuevo usuario */
			GetNewUserPack(data+header_size,&client);
			fprintf(stderr,"Llego un pedido de --new-- de user:%s passwd:%s\n",header.user,header.passwd);
			return UserRegister(client,socket);
			break;
		case __BUY_MOVIE__:
			/* Comprar pelicula */
			fprintf(stderr,"Llego un pedido de --buymovie-- de user:%s passwd:%s\n",header.user,header.passwd);
			GetBuyMoviePack(data+header_size,&buy);
			return UserBuyMovie(buy,socket,header.user,header.passwd);
			break;	
		case __DOWNLOAD__:
			/* Descargar pelicula */
			fprintf(stderr,"Llego un pedido de --download-- de user:%s passwd:%s\n",header.user,header.passwd);
			GetRequest(data+header_size,&req);
			fprintf(stderr,"--%s--\n",req.ticket);
			return UserDownload(req,socket, header.user, header.passwd);
			break;
		case __DOWNLOAD_START_OK__:
			/* Descargar pelicula */
			fprintf(stderr,"Llego un pedido de --startdownload-- de user:%s passwd:%s\n",header.user,header.passwd);
			GetDownloadStartOK(data+header_size,&start);
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

/*******************************************************************************************************/
/*                                   Funciones de atencion de pedidos                                  */
/*******************************************************************************************************/

/* case __USER_LOGIN__ */

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

/* case __NEW_PASSWD__ */

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

/* case __REG_USER__ */

status
UserRegister(client_t client,int socket)
{
	int ret = __REG_OK__;
	ack_t ack;
	
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

/* case __BUY_MOVIE__ */

status
UserBuyMovie(buy_movie_request_t buy,int socket,char *user,char *passwd)
{
	buy_movie_ticket_t ack;
	int ret = __BUY_MOVIE_OK__;
	void *ack_data;
	u_size ack_size;
	char *aux_ticket;
	
	/* Me fijo si esta logueado */
	if( strcmp(user, "anonimo") == 0 ) {
		ret = __USER_IS_NOT_LOG__;
	}
	/* Control de la identidad del solicitante */
	else if( !UserCanAcces(user, passwd) ) {
		ret = __USER_ACCESS_DENY__;
	}
	if( ret == __BUY_MOVIE_OK__ ) {
		if( PayMovie(buy.pay_name,buy.pay_user,buy.pay_passwd) == PAY_OK ) {
			if( (aux_ticket=MakeTicket(user,buy.movie_name)) == NULL )
				ret = __BUY_MOVIE_INVALID__;	
			else {
				strcpy(ack.ticket,MakeTicket(user,buy.movie_name));
				ret = __BUY_MOVIE_OK__;
			}
		}
		else {
			ret = __BUY_MOVIE_ERROR__;
		}	
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	ack_size = GetBuyTicketData(ack,&ack_data);
		
	sendTCP(socket, ack_data, ack_size);
	
	return OK;
}


/* case __DOWNLOAD__ */

status
UserDownload(request_t req,int socket,char *user,char *passwd)
{
	int ret = __DOWNLOAD_START__;
	download_header_t ack;
	ticket_info_t *file_info;
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
	
	fprintf(stderr, "ACA LLEGO GUACHIN\n");
	/* Busco la informacion del archivo, verifico permisos y nivel de descarga */
	if( ret == __DOWNLOAD_START__ ) {
		fprintf(stderr, "ACA TAMBIEN\n");
		fprintf(stderr, "%s\n",req.ticket);
		if( (file_info=GetFileInfo(req.ticket)) == NULL ) {
			fprintf(stderr, "PASO ALGO FULERO FULERO\n");	
			ret = __DOWNLOAD_ERROR__;
		}
		else {
			fprintf(stderr, "ESTA TODO RE PIOLA\n");
			strcpy(ack.title,file_info->path);
			ack.size = GetFileSize(file_info->path);
		}
	}
	/* Mando la respuesta */
	ack.ret_code = ret;
	size = GetDownloadHeaderData(ack,&data);
	sendTCP(socket, &ack, sizeof(download_header_t));
	return OK;
}

/* case __DOWNLOAD_START_OK__ */

status
UserStartDownload(download_start_t start,int socket, char *user, char *passwd)
{		
	ticket_info_t *file_info;
		
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
	if( (file_info=GetTicketInfo(start.ticket)) == NULL ) {
		/* Si no es valido no empiezo la descarga */
		return OK;
	}

	fprintf(stderr,"%s %s\n",start.ip,start.port);
	
	switch( fork() ) {
		case 0:
			/* Espero a que se establezca la conexion */
			sleep(2); //Cambiar esto porque es muy villero
			fprintf(stderr,"Empiezoooooo\n");
			if( SendMovie(file_info->path,start.ip,start.port) != OK )
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

/* case __LOG_OUT__ */

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

/*******************************************************************************************************/
/*                                          Static Functions                                           */
/*******************************************************************************************************/

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

/* Borro el usuario de la tabla de usuarios online */

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

/* Manda el archivo una vez establecida la conexion servidor -> cliente */

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
		memmove(to_send,header_data, header_size);
		memmove(to_send+header_size, data, bytes_read);
		num = sendTCP(socket,to_send,header_size+bytes_read);
			
		fprintf(stderr,"TCP %d\n", num);
		
		free(data);
		free(header_data);
		free(to_send);
	}
	
	close(socket);
	fclose(fd);
	fprintf(stderr,"Termine de transmitir\n");
	return OK;
}

static payment_server_t
SendPaymentServerLocationRequest( char *name )
{
	server_request_t req;
	void *req_data;
	u_size req_size;
	int socket;
	void *ack_data;
	payment_server_t ack;
	host_t lookup_server;
	
	strcpy(req.name,name);
	req_size = GetPaymentRequestData( req, &req_data);	
	socket = prepareUDP(PLS_IP, PLS_PORT);
	lookup_server.port=(unsigned short)atoi(PLS_PORT);
	strncpy(lookup_server.dir_inet,PLS_IP,DIR_INET_LEN);
	
	sendUDP(socket, req_data, lookup_server);

 	ack_data = receiveUDP(socket, &lookup_server);
	
	GetPaymentLocationPack(ack_data,&ack);
	
	fprintf(stderr,"%s %s %s %s\n",ack.name,ack.host,ack.port,ack.key);
		
	return ack;
}

static int
PayMovie(char *pay_name,char *pay_user,char *pay_passwd)
{
	//payment_server_t location = SendPaymentServerLocationRequest(pay_name);
		
	return PAY_OK;
}

static char *
MakeTicket(char *user,char *movie_name)
{
	ticket_info_t *ticket = malloc(sizeof(ticket_info_t));
	unsigned int ticket_number;
	char *ticket_string = malloc(MAX_TICKET_LEN);
	file_info_t * file;
	/* Genero el ticket */
	ticket_number = tickets_counter++;
	SaveCounter(tickets_counter,TICKETS_FREE_PATH);	
	sprintf(ticket_string, "10%d",ticket_number);
	/* Lo asocio a una descarga */
	if( (file=GetFileInfo(movie_name)) == NULL )
		return NULL;
	strcpy(ticket->path, file->path); 
	strcpy(ticket->MD5, file->MD5);
	strcpy(ticket->ticket, ticket_string);
	/* Inserto el ticket generado para su posterior uso */	
	HInsert(tickets_generated, ticket);
	SaveHashTable(tickets_generated, TICKETS_DATA_PATH);
	free(file);
	return ticket_string;
}

static file_info_t *
GetFileInfo(char *name)
{
	file_info_t file;
	file_info_t *file_ptr = malloc(sizeof(file_info_t));
	int pos;	
	
	strcpy(file.name, name);
	if( (pos=Lookup(file_paths, &file)) == -1 ) {
		fprintf(stderr, "No lo encontre carajo %d\n",pos);
		return NULL;
	}
	fprintf(stderr, "Lo encontre y esta en %d\n",pos);
	file_ptr=GetHElement(file_paths, pos);
	
	fprintf(stderr, "%s %s\n", file_ptr->name,file_ptr->path);
	
	return file_ptr;
}

static ticket_info_t *
GetTicketInfo(char *ticket)
{
	ticket_info_t file_info;
	ticket_info_t *file_ptr = malloc(sizeof(ticket_info_t));
	int pos;	
	
	strcpy(file_info.ticket, ticket);
	fprintf(stderr, "Voy a buscar --%s-- \n",file_info.ticket); 
	if( (pos=Lookup(tickets_generated, &file_info)) == -1 ) {
		fprintf(stderr, "No lo encontre carajo %d\n",pos);
		return NULL;
	}
	fprintf(stderr, "Lo encontre y esta en %d\n",pos);
	file_ptr=GetHElement(tickets_generated, pos);
	
	fprintf(stderr, "%s %s\n", file_ptr->ticket,file_ptr->path);
	
	return file_ptr;
}


/*******************************************************************************************************/
/*                                struct pack = GetPack(data)                                          */
/*******************************************************************************************************/

static u_size
GetPaymentLocationPack(void * ack_data,payment_server_t * ack)
{
    u_size pos=0;
    memmove(ack->name,ack_data,MAX_SERVER_LEN);
    pos+=MAX_SERVER_LEN;
    memmove(ack->host,ack_data+pos,MAX_HOST_LEN);
    pos+=MAX_HOST_LEN;
    memmove(ack->port,ack_data+pos,MAX_PORT_LEN);
    pos+=MAX_PORT_LEN;
    memmove(ack->key,ack_data+pos,MAX_SERVER_KEY);
    pos+=MAX_SERVER_KEY;
    
    return pos;
}

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
    memmove(download_start->ip,data,MAX_HOST_LEN);
    pos+=MAX_HOST_LEN;
    memmove(download_start->port,data+pos,MAX_PORT_LEN);
    pos+=MAX_PORT_LEN;
	memmove(download_start->port,data+pos,MAX_TICKET_LEN);
    pos+=MAX_TICKET_LEN;
    
    return pos;
}

static u_size
GetRequest(void *data, request_t * request)
{
    u_size pos;
    pos=0;
    memmove(request->ticket,data,MAX_TICKET_LEN);
    pos+=MAX_TICKET_LEN;
    
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
GetBuyMoviePack(void *data,buy_movie_request_t *buy)
{
	u_size pos;

	pos = 0;
	memmove(buy->movie_name, data, MAX_MOVIE_LEN);
	pos+=MAX_MOVIE_LEN;
	memmove(buy->pay_name, data + pos , MAX_SERVER_LEN);
	pos+=MAX_SERVER_LEN;
	memmove(buy->pay_user, data + pos , MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(buy->pay_passwd, data + pos , MAX_USER_PASS);
	pos+=MAX_USER_PASS;

	return pos;
}

/*******************************************************************************************************/
/*                                          GetData(pack)                                              */
/*******************************************************************************************************/
static u_size
GetPaymentRequestData(server_request_t req,void ** req_data)
{
    u_size pos;
    if( (*req_data=calloc(1,UDP_MTU))==NULL )
	return 0;

    memmove(*req_data,req.name,strlen(req.name));
    pos+=MAX_SERVER_LEN;

    return UDP_MTU;
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
GetBuyTicketData( buy_movie_ticket_t pack, void **data_ptr)
{
	void *data;
	u_size size;
	u_size pos;
	
	size = sizeof(unsigned long) + MAX_TICKET_LEN;
	
	if( (data=malloc(size)) == NULL )
		return -1;
	
	pos=0;
	memmove(data, &(pack.ret_code), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, pack.ticket, MAX_TICKET_LEN);
	pos+=MAX_TICKET_LEN;
	
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


