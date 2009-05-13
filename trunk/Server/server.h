/*
 *  server.h
 *  MovieStoreServer
 */

#ifndef __SERVER_H_
#define __SERVER_H_

#include "../Common/defines.h"
#include "../Common/app.h"


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

status NewClient(client_t client,int socket);

status UserNewPasswd(login_t log,int socket, char *user,char *passwd); 

/*************************************************************************/

int UsersComp( void *c1, void *c2 );

int UsersHash( void *c1, int size );

#endif
