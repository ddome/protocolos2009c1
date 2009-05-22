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
#define HOST "127.0.0.1"
#define PORT 389

/* Usuario root del ldap */
#define ADMIN_DN "cn=admin,dc=nodomain"
#define ADMIN_PASSWD "admin"

/* Client default */
#define CLIENT_KEY "uid"
#define CLIENT_PATH "ou=clients,dc=nodomain"

/* Movies */
#define MOVIE_KEY "uid"
#define MOVIE_PATH "ou=movies,o=movie_store.com"

/* Datos del servidor */
#define MOVIE_STORE_DN   "dc=nodomain"
#define MOVIE_STORE_NAME "nodomain"
#define MOVIE_STORE_LOCATION "Buenos Aires"

/* Funciones */

LDAP * InitLdap(void);

status ClientAdd(LDAP *ld, client_t client);

status ChangePasswd(LDAP *ld, char *user, char *passwd);

boolean UserExist(LDAP *ld, char *user);

boolean PasswdIsValid(LDAP *ld, char *user, char *passwd);

void EndLdap(LDAP *ld);

#endif
