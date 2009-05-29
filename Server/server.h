/*
 *  server.h
 *  MovieStoreServer
 */

#ifndef __SERVER_H_
#define __SERVER_H_

#include "../Common/defines.h"
#include "../Common/app.h"


#define HOST_SERVER "127.0.0.1"

#define PORT_SERVER   "1047"

#define PLS_IP "127.0.0.1"

#define PLS_PORT "1060"

typedef struct{
	movie_t movie;
	char path[MAX_PATH_LEN];	
} file_info_t;

/*************************************************************************/
/*					     Manejo del servidor        				     */
/*************************************************************************/


status InitServer(void);

status StartServer(void);

void EndServer(void);


/*************************************************************************/
/*					     Atencion de pedidos				             */
/*************************************************************************/

status Session(void *packet,int socket);

status UserLogin(login_t log,int socket);

status UserRegister(client_t client,int socket);

status UserNewPasswd(login_t log,int socket, char *user,char *passwd);

status UserLogout(int socket, char *user, char *passwd);

status UserDownload(request_t req,int socket,char *user,char *passwd);

status UserStartDownload(download_start_t start,int socket, char *user, char *passwd);

/*************************************************************************/

int UsersComp( void *c1, void *c2 );

int UsersHash( void *c1, int size );

#endif
