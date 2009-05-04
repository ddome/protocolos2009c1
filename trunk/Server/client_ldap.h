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

/* Direccion por default */
#define HOST "192.168.3.129"
#define PORT 389

/* Usuario root del ldap */
#define ADMIN_DN "uid=admin,o=movie_store.com"
#define ADMIN_PASSWD "admin"

/* Client default */
#define CLIENT_KEY "uid"
#define CLIENT_PATH "ou=clients,o=movie_store.com"

/* Funciones */

LDAP * InitLdap(void);

status ClientAdd(LDAP *ld, client_t client);

boolean UserExist(LDAP *ld, char *user);

boolean PasswdIsValid(LDAP *ld, char *user, char *passwd);

void EndLdap(LDAP *ld);

#endif
