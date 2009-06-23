/*
 *  server.h
 *  MovieStoreServer
 */

#ifndef __SERVER_H_
#define __SERVER_H_

#include "../Common/defines.h"
#include "../Common/app.h"
#include <signal.h>
#include <sys/types.h>
#include <syslog.h>

#define HOST_SERVER "127.0.0.1"

#define PORT_SERVER   "1048"

#define PORT_SERVER_UDP "1071"

#define HOST_LOOKUP "127.0.0.1"

#define PORT_LOOKUP "1070"

#define TICKETS_DATA_PATH "tickets_data"

#define TICKETS_FREE_PATH "tickets_free"

#define FILES_DATA_PATH "movies_location.txt"

#define DEFAULT_TIMEOUT 10

/*************************************************************************/
/*		            Manejo del servidor                          */
/*************************************************************************/


status InitServer(void);

status StartServer(void);

void EndServer(void);

/*************************************************************************/
/*                           Atencion de pedidos		 	 */
/*************************************************************************/

status Session(void *packet,int socket);

status UserLogin(login_t log,int socket);

status UserRegister(client_t client,int socket);

status UserNewPasswd(login_t log,int socket, char *user,char *passwd);

status UserLogout(int socket, char *user, char *passwd);

status UserBuyMovie(buy_movie_request_t buy,int socket,char *user,char *passwd);

status UserDownload(request_t req,int socket,char *user,char *passwd);

status UserStartDownload(download_start_t start,int socket, char *user, char *passwd);

status ListMoviesByGen(list_movie_request_t gen, int socket);

status ListUsers(int socket,char *user, char* passwd);

status ListGens(int socket);

/*************************************************************************/

int UsersComp( void *c1, void *c2 );

int UsersHash( void *c1, int size );

#endif
