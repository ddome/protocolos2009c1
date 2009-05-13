/*
 *    System includes
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 *    Proyect includes
 */
#include "scannerADT.h"
#include "tree.h"
#include "../Common/defines.h"
#include "client.h"

#define MAX_LINE 120


#define _COMMAND_NOT_VALID_  -1
#define _COMMAND_OK_		  1
#define _EXIT_				  2


/* Variable global con el nombre del usuario */
char user[150];

/* Funciones de utilidad
*/

static char * ReadLine( FILE * inputFile )
{
    char line[MAX_LINE+ 1];
    char * resp;
    int len;

    if( fgets( line, sizeof(line), inputFile ) ==NULL )
            return NULL;
    if( ((len = strlen( line )) == 0) )
        return NULL;
    line[len-1] = '\n';
    line[len] = '\0';

    resp = malloc( (len + 1) * sizeof(char) );
    strncpy(resp, line, len + 1);

    return (resp);
}


/* Comandos del Prompt
*/

static int Login_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1,*aux2;
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		aux2=ReadToken(scanner);
		
		switch( UserLogin(aux1, aux2) ) {
			case LOGIN_USER_INVALID:
				printf("El usuario es inexistente\n");
				break;
			case LOGIN_PASS_INVALID:
				printf("La clave es invalida para el usuario solicitado\n");
				break;
			case LOGIN_USER_IS_LOG:
				printf("El usuario %s ya esta logueado\n",aux1);
				break;
			case USER_LOGIN_OK:
				printf("%s, bienvenido a MovieStoreServer\n",aux1);
				strcpy(user, aux1);
				break;
			default:
				printf("Se ha producido un error al intentar conectarse al servidor\n");
		}
				
		free(aux1);
		free(aux2);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }

    return retValue;    
}

static int ChangePassword_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1,*aux2;
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		aux2=ReadToken(scanner);
		
		switch( UserChangePasswd(aux1, aux2) ) {
			case CHANGE_LOG_ERROR:
				printf("Debe estar logueado para realizar un cambio de contraseña\n");
				break;
			case NEW_PASSWD_INVALID:
				printf("La clave de seguridad no corresponde con la nueva clave ingresada\n");
				break;
			case CHANGE_ERROR:
				printf("Se produjo un error al intentar cambiar la clave de usuario\n");
				break;
			case CHANGE_OK:
				printf("Se ha cambiado su clave exitosamente\n");
				break;
			case CHANGE_ACCESS_DENY:
				printf("Acceso denegado\n");
				break;
			default:
				printf("Se ha producido un error al intentar conectarse al servidor\n");
		}
		
		free(aux1);
		free(aux2);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}


static int ShowCommands(scannerADT scanner, void * data)
{

    printf("Comandos disponibles:\n");
	printf("Login user password\n");
	printf("Password newpassword rep_newpassword\n");

    return OK;
}

/* Se cargan los comandos en el arbol de expresiones
*/

static void LoadTree(treeADT tree)
{
    InsertExpression(tree, "Login",  Login_Command);
	InsertExpression(tree, "Password",  ChangePassword_Command);
    InsertExpression(tree, "Help",   ShowCommands);
}

/* Prompt
*/

void
Prompt(void)
{
    char * strAux;
    int status = OK;
    int terminar = FALSE;
    treeADT tree;
    tree = NewTree();
    LoadTree(tree);

	strcpy(user, "anonimo");
    while ( !terminar )
    {
        printf("%s@client:~ $ ",user);
        strAux = ReadLine( stdin );
        if(strcmp(strAux, "\n") != 0)
            status = ReadExpression( tree, strAux, NULL );
        else
            status = 0;
        if(status < 0 )
            fprintf(stderr, "Comando invalido.\n");
		if( status == _EXIT_ ) {
			printf("Gracias por utilizar MovieStoreServer\n");
			terminar = TRUE;
		}
        free(strAux);
    }
    FreeTree(tree);
  /*  GoodBye();*/
}
	



