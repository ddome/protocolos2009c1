/*
 *  client.c
 *  MovieStoreClient
 *
 *  Created by Damian Dome on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "client.h"
#include "Prompt.h"
#include "../Common/cypher.h"
#include "../Common/fileHandler.h"

char log_user[MAX_USER_LEN];
char log_passwd[MAX_USER_PASS];

char client_host[MAX_HOST_LEN];
char client_port[MAX_PORT_LEN];

/* Static Functions */

static status NewDownload(int socket);

static unsigned long SendRequest(u_size op_code,u_size total_objects,void *packet, u_size size);

static unsigned long SendListMoviesRequest(void *data, u_size size, movie_t ***out_ptr);

static download_header_t SendDownloadRequest(void *packet, u_size size);

static buy_movie_ticket_t SendBuyRequest(void *packet, u_size size);

static status StartDownload(FILE *fd,char *ticket);

static status ListenMovie(FILE *fd,char *port,char *ticket);

static status ProcessDownload(void *packet);

static movie_t ** GetMovies(void *data, u_size number);

static int SendListUsersRequest(client_t ***out_ptr);


/* GetData(packet) */

static u_size GetLoginData( login_t pack, void ** data );

static u_size GetHeaderData( header_t pack, void **data_ptr);

static  u_size GetNewUserData( client_t pack, void **data_ptr);

static u_size GetRequestData( request_t pack, void **data_ptr);

static u_size GetDownloadStartData( download_start_t pack, void **data_ptr);

static u_size GetBuyData( buy_movie_request_t pack, void **data_ptr);

static u_size GetListMoviesData(list_movie_request_t pack, void **data_ptr);

/* GetPack(data) */

static u_size GetDownloadHeaderPack( void *data, download_header_t *pack );

static u_size GetDownloadPack( void *data, download_t *pack);

static u_size GetBuyTicketPack( void *data, buy_movie_ticket_t *pack);

static u_size GetHeaderPack(void *data, header_t *header);

static client_t ** GetUsersList(void *data, u_size number);

/*PID del proceso que escucha nuecas descargas*/
pid_t downloader_pid;
/*Socket destinado a las descaragas*/
int passive_s;

int
intHandler(int signum)
{
    close(passive_s);
    exit(EXIT_SUCCESS);
}

status 
InitClient(char *host,char *port)
{
	/* Cliente por default, sin privilegios */
	strcpy(log_user, "anonimo");
	strcpy(log_passwd, "anonimo");
	
	if( host != NULL ) {
		strcpy(client_host, host);
		printf("%s %s\n",host,port);
	}
	else
		strcpy(client_host, client_host);
	
	if( port != NULL ) {
		strcpy(client_port, port);
	}
	else
		strcpy(client_port, client_port);
	
	
	return OK;
}

status
StartClient(void)
{	
	switch(downloader_pid=fork()){
	
		case 0:
			if( InitDownloader() == OK)
				exit(EXIT_SUCCESS);
			else
				exit(EXIT_FAILURE);
			break;
		case -1:
			return FATAL_ERROR;
			break;
		default:
			Prompt();			
			break;
	}

	return OK;
}

void
EndClient(void)
{
	return;
}

void 
UserExit(void)
{
	kill(downloader_pid,SIGINT);
	if( strcmp(log_user, "anonimo") != 0 )
		UserLogout();
}

/* Escucha los pedidos de conexion para iniciar una descarga por parte del servidor */
status
InitDownloader(void)
{
	int ssock;
	signal(SIGINT,intHandler);
	/* Preparo el puerto que va a escuchar los pedidos de conexion de transferencia */
	if( (passive_s=prepareTCP(client_host,client_port,prepareServer)) < 0 ) {
		return FATAL_ERROR;
	}	
	if( (listenTCP(passive_s,10)) < 0 ) {
		return FATAL_ERROR;
	}
	
	while(1) {
		ssock = acceptTCP(passive_s);
		switch(fork()){
			case 0: 
				NewDownload(ssock);
				exit(EXIT_SUCCESS);
				break;
			case -1:
				return FATAL_ERROR;
				break;
			default:
				/* Sigo escuchando */
				break;
		}
	}	
	closeTCP(passive_s);
	
	return OK;
	
}

static status
NewDownload(int ssock)
{
	
	unsigned long total_packets;
	unsigned long n_packet;
	boolean exit;
	download_t header;

	void *packet;

	u_size header_size;
	FILE *fd;
		
	/* Recibo el primer paquete */
	n_packet = 0;
	exit = FALSE;
	packet = receiveTCP(ssock);
	header_size = GetDownloadPack(packet,&header);
	
	fd = fopen(header.title,"wb+");	
	/* Lo bajo a disco */
	PutFileData(fd,_FILE_SIZE_, header.n_packet,packet+header_size,header.size);
	/* Verifico la cantidad total de paquetes a descargar */
	total_packets = header.total_packets;
	free(packet);
	n_packet++;
	/* Me fijo si llego a la cantidad total de paquetes bajados */
	if( n_packet >= total_packets )
		exit = TRUE;
	
	while (!exit) {
		/* Recibo un paquete */
		packet = receiveTCP(ssock);
		header_size = GetDownloadPack(packet,&header);
		/* Lo bajo a disco */
		PutFileData(fd,_FILE_SIZE_, header.n_packet,packet+header_size,header.size);
		/* Verifico la cantidad total de paquetes a descargar */
		total_packets = header.total_packets;
		free(packet);
		n_packet++;
		/* Me fijo si llego a la cantidad total de paquetes bajados */
		if( n_packet >= total_packets )
			exit = TRUE;
	}
	fclose(fd);
	closeTCP(ssock);
	
	return OK;
}

client_login_status
UserLogin(char *user, char* passwd)
{
	int ret;
	int ret_code;
	login_t log;
	void *data;
	u_size size;
	
	/* Si estaba logueado con otra cuenta, lo deslogueo */
	if( strcmp(log_user, "anonimo") != 0 )
		UserLogout();
	
	/* Paquete del pedido */
	strcpy(log.user,user);
	strcpy(log.passwd,passwd);
	/* Mando el paquete */
	if( (size=GetLoginData(log, &data)) == -1 )
		return LOGIN_CONNECT_ERROR;
	ret_code = SendRequest(__USER_LOGIN__, 1, data, size);
	free(data);
	/* Proceso la respuesta */
	switch (ret_code) {
		case __LOGIN_OK__: 
		case __USER_IS_LOG__:
			/* Guardo los datos del usuario */
			strcpy(log_user,log.user);
			strcpy(log_passwd,log.passwd);
			ret = USER_LOGIN_OK;
			break;
		case __USER_ERROR__:
			ret = LOGIN_USER_INVALID;
			break;
		case __PASSWD_ERROR__:
			ret = LOGIN_PASS_INVALID;
			break;
		default:
			ret = LOGIN_CONNECT_ERROR;
			break;
	}
		
	return ret;
}

client_change_passwd_status
UserChangePasswd(char *new_passwd, char *rep_new_passwd)
{
	client_change_passwd_status ret;
	login_t new_client_info;
	int ret_code;
	char* sec_new_passwd;
	char * encripted;
	void * newPass_data;
	u_size size;
	
	/* Chequeo que ingrese la confirmacion de la clave correctamente */
	if( strcmp(new_passwd, rep_new_passwd) != 0 )
		return NEW_PASSWD_INVALID;
	/* Encripto la nueva clave */
	if( (sec_new_passwd=malloc(strlen(new_passwd)+1)) == NULL )
		return CHANGE_ERROR;
	/* Paquete de pedido */
	strcpy(new_client_info.passwd,new_passwd);
	strcpy(new_client_info.user,log_user);
	GetLoginData( new_client_info, &newPass_data);
	size=MAX_USER_LEN+MAX_USER_PASS;
	/*Encripto la informacion a enviar*/
	encripted=Cypher((char *)newPass_data,size,log_passwd);
	/* Mando el pedido */
	ret_code = SendRequest(	__NEW_PASSWD__, 1, encripted, size+(CYPHER_SIZE-size%CYPHER_SIZE));
	free(encripted);
	/* Proceso la respuesta */
	switch (ret_code) {
		case __CHANGE_OK__:
			ret = CHANGE_OK;
			/* Guardo la nueva contreaseña */
			strcpy(log_passwd, new_passwd);
			break;
		case __USER_IS_NOT_LOG__:
			ret = CHANGE_LOG_ERROR;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_ACCESS_DENY__:
			ret = CHANGE_ACCESS_DENY;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case CONNECT_ERROR:
			ret = CHANGE_ERROR;
			break;	
		default:
			ret = CHANGE_CONNECT_ERROR;
			break;
	}
	return ret;
}

client_buy_movie_status
UserBuyMovie(char *movie_name,char *pay_name,char *pay_user, char *pay_passwd, char *ticket_ret)
{
	buy_movie_request_t buy;
	void *buy_data;
	u_size buy_size;
	buy_movie_ticket_t ticket;
	client_buy_movie_status ret;
	char * encripted;
	
	/* Paquete de pedido */
	strcpy(buy.movie_name,movie_name);
	strcpy(buy.pay_name, pay_name);
	strcpy(buy.pay_user, pay_user);
	strcpy(buy.pay_passwd, pay_passwd);	
	buy_size = GetBuyData(buy,&buy_data);
	/*Encripto la informacion a enviar*/
	encripted=Cypher((char *)buy_data,buy_size,log_passwd);
	/* Mando el pedido */
	ticket = SendBuyRequest( encripted, buy_size+(CYPHER_SIZE-buy_size%CYPHER_SIZE));	
	/* Proceso la respuesta */
	switch (ticket.ret_code) {
		case __BUY_MOVIE_OK__:
			ret = BUY_OK;
			/* Devuelvo el ticket generado para imprimir en pantalla */
			strcpy(ticket_ret,ticket.ticket);
			break;
		case __USER_IS_NOT_LOG__:
			ret = BUY_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_ACCESS_DENY__:
			ret = BUY_ACCES_DENY;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __BUY_MOVIE_USER_ERROR__:
			ret = BUY_USER_ERROR;
			break;
		case __BUY_MOVIE_PASS_ERROR__:
			ret = BUY_PASS_ERROR;
			break;
		case __BUY_MOVIE_INVALID__:
			ret = BUY_INVALID_MOVIE;
			break;

		default:
			ret = BUY_CONNECT_ERROR;
			break;
	}
	return ret;
}

client_user_reg
UserRegistration(char *user, char *passwd, char *rep_passwd, char *mail, char *desc, int level)
{
	client_user_reg ret = REG_OK;
	client_t new_user;
	int ret_code;
	void *data;
	u_size size;

	/* Controlo que las clave de seguridad sea igual que la clave ingresada */
	if( strcmp(passwd, rep_passwd) != 0 ) {
		return REG_PASSWD_ERROR;
	}
	
	/* Armo el pedido */
	strcpy(new_user.user,user);
	strcpy(new_user.passwd,passwd);
	strcpy(new_user.mail,mail);
	strcpy(new_user.desc,desc);
	new_user.level = level;
	size = GetNewUserData(new_user, &data);		
	/* Mando el pedido */
	ret_code = SendRequest(__REG_USER__, 1, data, size);
	free(data);	
	/* Proceso la respuesta */
	switch (ret_code) {
		case __REG_USER_ERROR__:
			ret = REG_USER_EXISTS;
			break;
		case __REG_ERROR__:
			ret = REG_ERROR;
			break;
		case __REG_OK__:
			ret = REG_OK;
			break;
		case CONNECT_ERROR:
			ret = REG_ERROR;
			break;
		default:
			ret = LOGIN_CONNECT_ERROR;
			break;
	}
	
	return ret;
}

client_logout_status
UserLogout(void)
{
	client_logout_status ret = LOG_OUT_OK;
	int ret_code;
			
	/* Mando el pedido */
	ret_code = SendRequest(__LOG_OUT__, 1, NULL, 0);
	/* Proceso la respuesta */
	switch (ret_code) {
		case __LOG_OUT_ERROR__:
			ret = LOG_OUT_ERROR;
			break;
		case __USER_ACCESS_DENY__:
			ret = LOG_OUT_ACCES_DENY;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_IS_NOT_LOG__:
			ret = LOG_OUT_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __LOG_OUT_OK__:
			strcpy(log_user, "anonimo");
			strcpy(log_passwd, "anonimo");
			ret = LOG_OUT_OK;
			break;
		default:
			ret = LOG_OUT_CONNECT_ERROR;
			break;
	}
	
	return ret;
}

client_download_status
UserDownload(char * ticket)
{
	client_download_status ret = DOWNLOAD_OK;
	request_t request;
	download_header_t download_info;
	FILE *fd;
	void *data;
	u_size size;
	
	strcpy(request.ticket,ticket);
	size = GetRequestData(request,&data);
	fprintf(stderr, "%s\n",data);
	/* Mando el pedido */
	download_info = SendDownloadRequest(data, size);
	free(data);
	/* Proceso la respuesta */
	switch (download_info.ret_code) {
		case __DOWNLOAD_ERROR__:
			ret = DOWNLOAD_ERROR;
			break;
		case __USER_ACCESS_DENY__:
			ret = DOWNLOAD_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __USER_IS_NOT_LOG__:
			ret = DOWNLOAD_USER_NOT_LOG;
			/* El usuario debe loguearse devuelta */
			if( strcmp(log_user, "anonimo") != 0 )
				strcpy(log_user, "anonimo");
			break;
		case __DOWNLOAD_START__:
			ret = DOWNLOAD_OK;
			/* Reservo espacio en el disco local para el archivo que voy a bajar */
			printf("(%s)\n",download_info.title);
			if( (fd = CreateFile(download_info.title,download_info.size)) == NULL )
				ret = DOWNLOAD_ERROR;
			/* Comienzo a descargar */
			else if( StartDownload(fd,ticket) == ERROR )
				ret = DOWNLOAD_ERROR;
			break;
		default:
			ret = DOWNLOAD_CONNECT_ERROR;
			break;
	}
	
	return ret;
}

client_list_movies_by_gen_status
ListMoviesByGen(char *gen, movie_t ***movie_list_ptr)
{
	client_list_movies_by_gen_status ret = LIST_OK;
	list_movie_request_t list_movies_request;

	void *data;
	u_size size;
	u_size n_movies;
	movie_t **movies_list;
		
	/* Armo el pedido */				
	strcpy(list_movies_request.gen,gen);
	size = GetListMoviesData(list_movies_request, &data);		
	/* Mando el pedido */
	if( (n_movies = SendListMoviesRequest(data, size, &movies_list)) < 0 )
		ret = LIST_ERROR;
	else {
		if( n_movies == 0 ) {
			ret = LIST_ERROR;
		}
		else{
		*movie_list_ptr = movies_list;
		ret = LIST_OK;
		}
	}
	free(data);	
	return ret;
}

list_users_status
ListUsers(client_t ***users_list_ptr)
{
	list_users_status ret = LIST_USERS_OK;
	
	int n_users;
	client_t **users_list;
	
	/* Mando el pedido */
	if( (n_users=SendListUsersRequest(&users_list)) < 0 )
		return LIST_USERS_ERROR;
	else {
		if( n_users == 0 ) {
			*users_list_ptr = NULL;
			ret = LIST_USERS_OK;
		}
		else{
			*users_list_ptr = users_list;
			ret = LIST_USERS_OK;
		}
	}
	
	return ret;
}

/* Static Functions */

static int
SendListUsersRequest(client_t ***out_ptr)
{
	header_t header;
	header_t ack_header;
	void *ack_users;
	int socket;
	void *header_data;
	u_size header_size;
	
	/* Tipo de pedido */
	header.opCode = __LIST_USERS__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	header_size = GetHeaderData(header, &header_data);	
	
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(header_data);
		return -1;
	}
	/* Mando el paquete */
	sendTCP(socket, header_data,header_size);
	free(header_data);
	
	/* Espero por la respuesta del servidor */
	GetHeaderPack(receiveTCP(socket),&ack_header);
	
	if( ack_header.opCode == __LIST_USERS_OK__ ) {
		
		ack_users = receiveTCP(socket);
		*out_ptr = GetUsersList(ack_users,ack_header.total_objects);
		free(ack_users);
	}
	else {
		return -1;
	}
		
	
	close(socket);
	return ack_header.total_objects;	
	
}

static unsigned long
SendListMoviesRequest(void *data, u_size size, movie_t ***out_ptr)
{
	header_t header;
	header_t ack_header;
	void *ack_movies;
	void *to_send;
	int socket;
	void *header_data;
	u_size header_size;
	
	/* Tipo de pedido */
	header.opCode = __LIST_MOVIES_BY_GEN__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	header_size = GetHeaderData(header, &header_data);	
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(header_size+size)) == NULL )
		return CONNECT_ERROR;	
	memmove(to_send, header_data, header_size);
	free(header_data);
	/* Chequeo si realmente se manda un paquete asociado al pedido */
	
	memmove(to_send+header_size, data, size);
	
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return CONNECT_ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,header_size+size);
	free(to_send);
	
	/* Espero por la respuesta del servidor */
	GetHeaderPack(receiveTCP(socket),&ack_header);
	
	if( ack_header.opCode == __LIST_OK__ ) {
		ack_movies = receiveTCP(socket);
		*out_ptr = GetMovies(ack_movies,ack_header.total_objects);
		free(ack_movies);
	}
	else
		ack_header.total_objects = 0;

	/* Cierro la conexion????? */
	close(socket);
	return ack_header.total_objects;
}

static movie_t **
GetMovies(void *data, u_size number)
{
	int i;
	movie_t **list = malloc(sizeof(movie_t*)*(number+1));
	
	u_size pos = 0;
	for(i=0;i<number;i++){
		
		list[i] = malloc(sizeof(movie_t));
		
		memmove(list[i]->name, data+pos, MAX_MOVIE_LEN);
		pos+=MAX_MOVIE_LEN;
		memmove(list[i]->gen, data+pos, MAX_MOVIE_GEN);
		pos+=MAX_MOVIE_GEN;
		memmove(list[i]->plot, data+pos, MAX_MOVIE_PLOT);
		pos+=MAX_MOVIE_PLOT;
		memmove(&(list[i]->duration), data+pos, sizeof(u_size));
		pos+=sizeof(u_size);
		memmove(&(list[i]->size), data+pos, sizeof(u_size));
		pos+=sizeof(u_size);
		memmove(&(list[i]->value), data+pos, sizeof(u_size));
		pos+=sizeof(u_size);
		memmove(list[i]->MD5, data+pos, M_SIZE);
		pos+=M_SIZE;		
	}
	
	list[i] = NULL;
	return list;	
}

static client_t **
GetUsersList(void *data, u_size number)
{
	int i;
	client_t **list = malloc(sizeof(client_t*)*(number+1));
	
	u_size pos = 0;
	for(i=0;i<number;i++){
		
		list[i] = malloc(sizeof(client_t));
		
		memmove(list[i]->user, data+pos, MAX_USER_LEN);
		pos+=MAX_USER_LEN;
		memmove(list[i]->mail, data+pos, MAX_USER_MAIL);
		pos+=MAX_USER_MAIL;
		memmove(list[i]->desc, data+pos, MAX_USER_DESC);
		pos+=MAX_USER_DESC;
		memmove(&(list[i]->level), data+pos, sizeof(unsigned char));
		pos+=sizeof(unsigned char);

	}
	
	list[i] = NULL;
	return list;	
}

static unsigned long 
SendRequest(u_size op_code,u_size total_objects,void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	int ret;
	ack_t *ack_ptr;
	void *data;
	u_size header_size;
	
	/* Tipo de pedido */
	header.opCode = op_code;
	header.total_objects = total_objects;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	header_size = GetHeaderData(header, &data);	
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(header_size+size)) == NULL )
		return CONNECT_ERROR;	
	memmove(to_send, data, header_size);
	free(data);
	/* Chequeo si realmente se manda un paquete asociado al pedido */
	if( packet != NULL ) {
		memmove(to_send+header_size, packet, size);
	}	
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return CONNECT_ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,header_size+size);
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_ptr = receiveTCP(socket);	
	ret = ack_ptr->ret_code;
	free(ack_ptr);
	/* Cierro la conexion????? */
	close(socket);
	return ret;
}

static download_header_t
SendDownloadRequest(void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	download_header_t download_info;
	void *data;
	u_size header_size;
	void *ack_data;
	
	download_info.ret_code = CONNECT_ERROR;
	
	/* Tipo de pedido */
	header.opCode = __DOWNLOAD__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);	
	header_size = GetHeaderData(header, &data);	
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(header_size+size)) == NULL )
		return download_info;	
	memmove(to_send, data, header_size);
	memmove(to_send+header_size, packet, size);
	free(data);
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return download_info;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,header_size+size);
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_data = receiveTCP(socket);	
	GetDownloadHeaderPack(ack_data, &download_info);
	/* Cierro la conexion????? */
	close(socket);
	return download_info;
	
}

static buy_movie_ticket_t
SendBuyRequest(void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	buy_movie_ticket_t download_info;
	void *data;
	u_size header_size;
	void *ack_data;
	
	download_info.ret_code = BUY_ERROR;
	
	/* Tipo de pedido */
	header.opCode = __BUY_MOVIE__;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);	
	header_size = GetHeaderData(header, &data);	
	/* Concateno los paquetes header y pedido */
	if( (to_send = malloc(header_size+size)) == NULL )
		return download_info;	
	memmove(to_send, data, header_size);
	memmove(to_send+header_size, packet, size);	
	free(data);
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return download_info;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,header_size+size);
	free(to_send);
	/* Espero por la respuesta del servidor */
	ack_data = receiveTCP(socket);	
	GetBuyTicketPack(ack_data, &download_info);
	/* Cierro la conexion????? */
	close(socket);
	return download_info;	
}

static status
SendSignal(u_size op_code, void *packet, u_size size)
{
	header_t header;
	void *to_send;
	int socket;
	u_size header_size;
	void *data;
	
	/* Tipo de senial */
	header.opCode = op_code;
	header.total_objects = 1;
	/* Identificacion del usuario */
	strcpy(header.user,log_user);
	strcpy(header.passwd,log_passwd);
	/* Concateno los paquetes header y pedido */
	header_size = GetHeaderData(header, &data);	
	if( (to_send = malloc(header_size+size)) == NULL )
		return ERROR;	
	memmove(to_send, data, header_size);
	memmove(to_send + header_size,packet,size);
	free(data);
	/* Me conecto al servidor */
	if( (socket=connectTCP(HOST_SERVER,PORT_SERVER)) < 0 ){
		free(to_send);
		return ERROR;
	}
	/* Mando el paquete */
	sendTCP(socket, to_send,header_size+size);
	free(to_send);
	
	close(socket);
	return OK;	
}

static status
ListenMovie(FILE *fd,char *port,char *ticket)
{
	int passive_s,ssock;
	unsigned long total_packets;
	unsigned long n_packet;
	boolean exit;
	download_t header;
	download_start_t start;
	void *packet;
	void *data;
	u_size size;
	u_size header_size;
	
	/* Preparo el puerto que va a escuchar la conexion */
	if( (passive_s=prepareTCP(client_host,port,prepareServer)) < 0 ) {
		return FATAL_ERROR;
	}	
	if( (listenTCP(passive_s,10)) < 0 ) {
		return FATAL_ERROR;
	}
	
	/* Mando la senial al server pidiendo el inicio de la descarga */
	strcpy(start.port,port);
	strcpy(start.ip,client_host);
	strcpy(start.ticket,ticket);
	
	size = GetDownloadStartData(start, &data);	
	if( SendSignal(__DOWNLOAD_START_OK__, data, size) == ERROR )
		return ERROR;
	free(data);
	
	exit = FALSE;
	n_packet = 0;
	ssock=acceptTCP(passive_s);
	while (!exit) {
		/* Recibo un paquete */
		packet = receiveTCP(ssock);
		header_size = GetDownloadPack(packet,&header);
		fprintf(stderr,"caca: (%ld) (%ld)\n",header.n_packet,n_packet);
		/* Lo bajo a disco */
		PutFileData(fd,_FILE_SIZE_, header.n_packet,packet+header_size,header.size);
		/* Verifico la cantidad total de paquetes a descargar */
		total_packets = header.total_packets;
		free(packet);
		n_packet++;
		/* Me fijo si llego a la cantidad total de paquetes bajados */
		if( n_packet >= total_packets )
			exit = TRUE;
		fprintf(stderr,"caca: (%ld) (%ld)\n",header.n_packet,n_packet);
	}
	closeTCP(ssock);
	closeTCP(passive_s);
	
	return OK;
}

static status 
StartDownload(FILE *fd,char *ticket)
{	
	download_start_t start;
	void *data;
	u_size size;
	
	/* Mando la senial al server pidiendo el inicio de la descarga */
	strcpy(start.port,client_port);
	strcpy(start.ip,client_host);
	strcpy(start.ticket,ticket);
	
	size = GetDownloadStartData(start, &data);	
	if( SendSignal(__DOWNLOAD_START_OK__, data, size) == ERROR )
		return ERROR;
	free(data);
	return OK;
}

static u_size
GetHeaderData( header_t pack, void **data_ptr) 
{
	void *data;
	u_size size;
	u_size pos;
	
	size = sizeof(unsigned long) * 2 + MAX_USER_LEN + MAX_USER_PASS;
	
	if( (data=malloc(size)) == NULL )
		return -1;
	
	pos=0;
	memmove(data, &(pack.opCode), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, &(pack.total_objects), sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(data+pos, pack.user, MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(data+pos, pack.passwd, MAX_USER_PASS);
	
	*data_ptr = data;
	return size;
}

static  u_size
GetNewUserData( client_t pack, void **data_ptr)
{
	void *data;
	u_size size;
	u_size pos;
	
	size = MAX_USER_LEN  + MAX_USER_PASS + 
		   MAX_USER_MAIL + MAX_USER_DESC + sizeof(unsigned char);
	
	if( (data=malloc(size)) == NULL )
		return -1;
	
	pos=0;
	memmove(data, pack.user, MAX_USER_LEN);
	pos+=MAX_USER_LEN;
	memmove(data+pos, pack.passwd, MAX_USER_PASS);
	pos+=MAX_USER_PASS;
	memmove(data+pos, pack.mail, MAX_USER_MAIL);
	pos+=MAX_USER_MAIL;
	memmove(data+pos, pack.desc, MAX_USER_DESC);
	pos+=MAX_USER_DESC;
	memmove(data+pos, &pack.level, sizeof(unsigned char));
	pos+=sizeof(unsigned char);
	
	*data_ptr = data;
	return size;	
}

static u_size
GetListMoviesData(list_movie_request_t pack, void **data_ptr)
{
	void *data;
	
	if( (data=malloc(MAX_MOVIE_GEN)) == NULL )
		return -1;
	
	memmove(data, pack.gen, MAX_MOVIE_GEN);
	
	*data_ptr = data;
	return MAX_MOVIE_GEN;	
	
}
static u_size
GetLoginData( login_t pack, void **data_ptr) 
{
	void *data;
	
	if( (data=malloc(MAX_USER_LEN + MAX_USER_PASS)) == NULL )
		return -1;
	
	memmove(data, pack.user, MAX_USER_LEN);
	memmove(data + MAX_USER_LEN, pack.passwd, MAX_USER_PASS);
	
	*data_ptr = data;
	return MAX_USER_LEN + MAX_USER_PASS;
}

static u_size
GetRequestData( request_t pack, void **data_ptr) 
{
	void *data;
	
	if( (data=malloc(MAX_TICKET_LEN)) == NULL )
		return -1;
	
	memmove(data, pack.ticket, MAX_TICKET_LEN);
	
	*data_ptr = data;
	return MAX_TICKET_LEN;
}

static u_size
GetDownloadStartData( download_start_t pack, void **data_ptr) 
{
	void *data;
	u_size pos;
	
	if( (data=malloc(MAX_HOST_LEN + MAX_PORT_LEN + MAX_TICKET_LEN)) == NULL )
		return -1;
	
	pos = 0;
	memmove(data, pack.ip, MAX_HOST_LEN);
	pos += MAX_HOST_LEN;
	memmove(data + pos, pack.port, MAX_PORT_LEN);
	pos += MAX_PORT_LEN;
	memmove(data + pos, pack.port, MAX_TICKET_LEN);
	pos += MAX_TICKET_LEN;	
	
	*data_ptr = data;
	return pos;
}


static u_size
GetBuyData( buy_movie_request_t pack, void **data_ptr) 
{
	void *data;
	u_size pos;
	
	if( (data=malloc(MAX_MOVIE_LEN+MAX_SERVER_LEN+MAX_USER_LEN+MAX_USER_PASS)) == NULL )
		return -1;
	
	pos = 0;
	memmove(data, pack.movie_name, MAX_MOVIE_LEN);
	pos += MAX_MOVIE_LEN;
	memmove(data + pos, pack.pay_name, MAX_SERVER_LEN);
	pos += MAX_SERVER_LEN;
	memmove(data + pos, pack.pay_user, MAX_USER_LEN);
	pos += MAX_USER_LEN;
	memmove(data + pos, pack.pay_passwd, MAX_USER_PASS);
	pos += MAX_USER_PASS;
	
	*data_ptr = data;
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
GetDownloadHeaderPack( void *data, download_header_t *pack )
{
	u_size pos;
	
	pos = 0;
	memmove(&(pack->ret_code), data, sizeof(int) );
	pos+=sizeof(int);
	memmove(pack->title, data + pos , MAX_MOVIE_LEN);
	pos+=MAX_MOVIE_LEN;
	memmove(pack->path, data + pos , MAX_PATH_LEN);
	pos+=MAX_PATH_LEN;
	memmove(&(pack->total_packets), data + pos , sizeof(unsigned long));
	pos+=sizeof(unsigned long);
	memmove(&(pack->size), data + pos , sizeof(u_size));
	pos+=sizeof(u_size);
	
	return pos;
}

static u_size
GetBuyTicketPack( void *data, buy_movie_ticket_t *pack)
{
	u_size pos;
	
	pos = 0;
	memmove(&(pack->ret_code), data, sizeof(unsigned long) );
	pos+=sizeof(sizeof(unsigned long));
	memmove(pack->ticket, data + pos, MAX_TICKET_LEN );
	pos+=MAX_TICKET_LEN;
	
	return pos;
}

static u_size
GetDownloadPack( void *data, download_t *pack)
{
	u_size pos;
	
	pos = 0;
	memmove(&(pack->n_packet), data, sizeof(unsigned long) );
	pos+=sizeof(unsigned long);
	memmove(&(pack->total_packets), data + pos,sizeof(unsigned long) );
	pos+=sizeof(unsigned long);
	memmove(pack->title, data + pos,MAX_MOVIE_LEN );
	pos+=MAX_MOVIE_LEN;
	memmove(&(pack->size), data + pos , sizeof(u_size));
	pos+=sizeof(u_size);
	
	return pos;
}

