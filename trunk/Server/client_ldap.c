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

static status RootAdd(LDAP *ld);
static status ClientsInit(LDAP *ld);
static boolean RootExists(LDAP *ld);
static boolean ClientListExists(LDAP *ld);

LDAP * 
InitLdap(void)
{
	LDAP *ld;
	
	/* Conectarse al servidor */
	ld = ldap_open(HOST,PORT);
	if( ld == NULL ){
		fprintf(stderr,"InitLdap ERROR: No pudo establecerce una coneccion con el servidor LDAP en el host %s:%d\n",HOST,PORT);
	}
	
	/* Se identifica al usuario root */
	if( ldap_bind_s(ld,ADMIN_DN,ADMIN_PASSWD,LDAP_AUTH_SIMPLE) != LDAP_SUCCESS ){
		fprintf(stderr,"InitLdap ERROR: El servidor LDAP rechazo el pedido de autentificacion del usuario root %d\n",ADMIN_DN);		
	}
	
	/* Se inicializa la base de datos */
	if( !RootExists(ld) ) {
		if( RootAdd(ld) == FATAL_ERROR ) {
			EndLdap(ld);
			return NULL;
		}
	}
	if( !ClientListExists(ld) ) {
		if( ClientsInit(ld) == FATAL_ERROR ) {
			EndLdap(ld);
			return NULL;
		}
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
	
	if ( ldap_add_s(ld, dn, attributes) != LDAP_SUCCESS ) {
		fprintf(stderr,"LDAP ERROR: No pudo agregarse el cliente\n");
		return ERROR;
	}
	
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

status
ChangePasswd(LDAP *ld, char *user, char *passwd)
{
	LDAPMod *attributes[2];
	LDAPMod attribute1;
	
	char * pass_value[]		  = {passwd,NULL};
	
	char *dn = GetClientDN(user);
	
	/* userPassword */	
	attribute1.mod_op = LDAP_MOD_REPLACE;
	attribute1.mod_type = "userPassword";
	attribute1.mod_vals.modv_strvals = pass_value;	
	attributes[0] = &attribute1;
	
	attributes[1] = NULL;
	
	if ( ldap_modify_s(ld, dn, attributes) != LDAP_SUCCESS ) {
		fprintf(stderr,"LDAP ERROR: No pudo cambiarse la clave del cliente\n");
		return ERROR;
	}
	
	//free(dn);
	
	return OK;
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
	   
static boolean 
RootExists(LDAP *ld)
{	  
	struct berval bvalue;
		
	if( ld == NULL )
		return FALSE;
	
	bvalue.bv_val = MOVIE_STORE_NAME;
	bvalue.bv_len = strlen(MOVIE_STORE_NAME);
	
	if(ldap_compare_ext_s( ld, MOVIE_STORE_DN, "o", &bvalue, NULL, NULL ) == LDAP_COMPARE_TRUE )
		return TRUE;
	else
		return FALSE;	
}
	   

static status 
RootAdd(LDAP *ld)
{
	LDAPMod *attributes[4];
	LDAPMod attribute1,attribute2,attribute3;
	
	char * objectclass_vals[] = { "top","organization",NULL };
	char * o_value[]        = {MOVIE_STORE_NAME,NULL};
	char * l_value[]        = {MOVIE_STORE_LOCATION,NULL};
	
	/* objectclass */
	
	attribute1.mod_op = LDAP_MOD_ADD;
	attribute1.mod_type = "objectclass";
	attribute1.mod_vals.modv_strvals = objectclass_vals;	
	attributes[0] = &attribute1;
	
	/* o */
	
	attribute2.mod_op = LDAP_MOD_ADD;
	attribute2.mod_type = "o";
	attribute2.mod_vals.modv_strvals = o_value;	
	attributes[1] = &attribute2;
	
	/* l */
	
	attribute3.mod_op = LDAP_MOD_ADD;
	attribute3.mod_type = "l";
	attribute3.mod_vals.modv_strvals = l_value;	
	attributes[2] = &attribute3;
	
	attributes[3] = NULL;
		
	if ( ldap_add_s(ld, MOVIE_STORE_DN, attributes) != LDAP_SUCCESS ) {
		fprintf(stderr,"LDAP ERROR: No se pudo agregar el nodo inicial %s\n", MOVIE_STORE_DN);
		return FATAL_ERROR;
	}
		
	return OK;	
}

static boolean 
ClientListExists(LDAP *ld)
{	  
	struct berval bvalue;
	
	if( ld == NULL )
		return FALSE;
	
	bvalue.bv_val = "clients";
	bvalue.bv_len = strlen("clients");
	
	if(ldap_compare_ext_s( ld, CLIENT_PATH, "ou", &bvalue, NULL, NULL ) == LDAP_COMPARE_TRUE )
		return TRUE;
	else
		return FALSE;	
}

static status 
ClientsInit(LDAP *ld)
{
	LDAPMod *attributes[4];
	LDAPMod attribute1,attribute2,attribute3;
	
	char * objectclass_vals[] = { "top","organizationalUnit",NULL };
	char * ou_value[]        = {"clients",NULL};
	char * desc_value[]        = {"Lista de clientes",NULL};
	
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
	
	/* description */
	
	attribute3.mod_op = LDAP_MOD_ADD;
	attribute3.mod_type = "description";
	attribute3.mod_vals.modv_strvals = desc_value;	
	attributes[2] = &attribute3;
	
	attributes[3] = NULL;
	
	if ( ldap_add_s(ld, CLIENT_PATH, attributes) != LDAP_SUCCESS ) {
		fprintf(stderr,"LDAP ERROR: No se pudo inicializar la lista de clientes %s\n", CLIENT_PATH);
		return FATAL_ERROR;
	}
	
	
	return OK;			
}	