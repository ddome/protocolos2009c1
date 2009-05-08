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

	return ld;
}

status 
ClientAdd(LDAP *ld, client_t client)
{
	LDAPMod *attributes[9];
	LDAPMod attribute1,attribute2,attribute3,attribute4,attribute5,attribute6,attribute7,attribute8;
	
	char * objectclass_vals[] = { "top", "person", "organizationalPerson", "inetOrgPerson", NULL };
	char *  ou_value[]		  = {"clients",NULL};
	char * cn_value[]         = {client.user,NULL};
	char * uid_value[]        = {client.user,NULL};
	char * mail_value[]       = {client.mail,NULL};
	char * pass_value[]		  = {client.passwd,NULL};
	char * sn_value[]		  = {client.user,NULL};
	char * desc_value[]		  = {client.desc,NULL};
	
	char *dn = GetClientDN(client.user);
	
	/* objectclass */
	
	attribute1.mod_op = LDAP_MOD_ADD;
	attribute1.mod_type = "objectclass";
	attribute1.mod_vals.modv_strvals = objectclass_vals;	
	attributes[0] = &attribute1;

	/* ou */
	
	attribute2.mod_op = LDAP_MOD_ADD;
	attribute2.mod_type = "ou";
	attribute2.mod_vals.modv_strvals = ou_value;	
	attributes[1] = &attribute2;
	
	/* cn */
	
	attribute3.mod_op = LDAP_MOD_ADD;
	attribute3.mod_type = "cn";
	attribute3.mod_vals.modv_strvals = cn_value;	
	attributes[2] = &attribute3;
	
	/* uid */
	
	attribute4.mod_op = LDAP_MOD_ADD;
	attribute4.mod_type = "uid";
	attribute4.mod_vals.modv_strvals = uid_value;	
	attributes[3] = &attribute4;
	
	/* mail */
	
	attribute5.mod_op = LDAP_MOD_ADD;
	attribute5.mod_type = "mail";
	attribute5.mod_vals.modv_strvals = mail_value;	
	attributes[4] = &attribute5;
	
	/* userPassword */
	
	attribute6.mod_op = LDAP_MOD_ADD;
	attribute6.mod_type = "userPassword";
	attribute6.mod_vals.modv_strvals = pass_value;	
	attributes[5] = &attribute6;
	
	/* sn */
	
	attribute7.mod_op = LDAP_MOD_ADD;
	attribute7.mod_type = "sn";
	attribute7.mod_vals.modv_strvals = sn_value;	
	attributes[6] = &attribute7;
	
	/* description */
	
	attribute8.mod_op = LDAP_MOD_ADD;
	attribute8.mod_type = "description";
	attribute8.mod_vals.modv_strvals = desc_value;	
	attributes[7] = &attribute8;
	
	/* Fin del arreglo */
	attributes[8] = NULL;
	
	if ( ldap_add_s(ld, dn, attributes) != LDAP_SUCCESS )
		fprintf(stderr,"LDAP ERROR: No pudo agregarse el cliente\n");
	
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

