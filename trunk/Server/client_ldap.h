/*
*  client_ldap.h
*  MovieStoreServer
*/

#ifndef __CLIENT_LDAP_H__
#define __CLIENT_LDAP_H__

/* System includes */
#include <ldap.h>

/* Project includes */
#include "../Common/defines.h"
#include "../Common/app.h"

/* Direccion por default */
#define HOST "172.16.96.129" 
#define PORT 389

/* Usuario root del ldap */
#define ADMIN_DN "uid=admin,o=movie_store.com"
#define ADMIN_PASSWD "admin"

/* Client default */
#define CLIENT_KEY "uid"
#define CLIENT_PATH "ou=clients,o=movie_store.com"

/* Datos del servidor */
#define MOVIE_STORE_DN   "o=movie_store.com"
#define MOVIE_STORE_NAME "movie_store.com"
#define MOVIE_STORE_LOCATION "Buenos Aires"

/* Funciones */

LDAP * InitLdap(void);

status ClientAdd(LDAP *ld, client_t client);

status ChangePasswd(LDAP *ld, char *user, char *passwd);

boolean UserExist(LDAP *ld, char *user);

boolean PasswdIsValid(LDAP *ld, char *user, char *passwd);

unsigned int GetUserLevel(LDAP *ld,char *user);

void EndLdap(LDAP *ld);

#endif
