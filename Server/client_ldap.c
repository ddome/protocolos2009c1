/*
 *  client_ldap.c
 *  MovieStoreServer
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "client_ldap.h"

/* Static functions */

static char * GetClientDN(char *user);

LDAP * 
InitLdap(void)
{
	LDAP *ld;
	
	ld = ldap_open(HOST,PORT);
	if( ld == NULL ){
		fprintf(stderr,"InitLdap ERROR: No pudo establecerce una coneccion con el servidor LDAP en el host %s:%d\n",HOST,PORT);
	}
	
	if( ldap_bind_s(ld,ADMIN_DN,ADMIN_PASSWD,LDAP_AUTH_SIMPLE) != LDAP_SUCCESS ){
		fprintf(stderr,"InitLdap ERROR: El servidor LDAP rechazo el pedido de autentificacion del usuario root %d\n",ADMIN_DN);		
	}
	
	GetClientDN("cacastro");
	
	return ld;
}

status 
ClientAdd(LDAP *ld, client_t client)
{
	return OK;
}


boolean 
UserExist(LDAP *ld,char *user)
{
	struct berval bvalue;
	int rc;
	char *aux;
	const char *compAttribute = CLIENT_KEY;	
	
	if( ld == NULL || user == NULL )
		return FALSE;

	bvalue.bv_val = user;
	bvalue.bv_len = strlen(user);
	
	aux = GetClientDN(user);
	rc = ldap_compare_ext_s( ld, aux, compAttribute, &bvalue, NULL, NULL );
	free(aux);
	
	if( rc == LDAP_COMPARE_TRUE )
		return TRUE;
	else
		return FALSE;	
}

boolean 
PasswdIsValid(LDAP *ld, char *user, char *passwd)
{
	struct berval bvalue;
	int rc;
	char *aux;
	const char *compAttribute = "userPassword";	
	
	if( ld == NULL || user == NULL || passwd == NULL )
		return FALSE;
	
	bvalue.bv_val = passwd;
	bvalue.bv_len = strlen(passwd);
	
	aux = GetClientDN(user);
	rc = ldap_compare_ext_s( ld, aux, compAttribute, &bvalue, NULL, NULL );
	free(aux);
	
	if( rc == LDAP_COMPARE_TRUE )
		return TRUE;
	else
		return FALSE;	
}

void 
EndLdap(LDAP *ld)
{
	ldap_unbind(ld);
}


/* Static Functions */

static char *
GetClientDN(char *user)
{
	char *aux;
	
	aux = malloc(strlen(CLIENT_KEY) +
				 strlen("=")		+
				 strlen("user")		+
				 strlen(",")		+
				 strlen(CLIENT_PATH) + 1);
	
	strcpy(aux, CLIENT_KEY);
	strcat(aux, "=");
	strcat(aux, user);
	strcat(aux, ",");
	strcat(aux, CLIENT_PATH);
	
	return aux;	
}

