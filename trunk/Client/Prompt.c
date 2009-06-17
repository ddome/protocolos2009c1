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


static int Exit_Command(scannerADT scanner, void * data)
{
	UserExit();
	printf("Gracias por utilizar MovieStoreServer\n");

	return _EXIT_;
}


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
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
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
				printf("El usuario ha sido deslogueado desde otra terminal\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
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

static int NewAccount_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1,*aux2,*aux3,*aux4,*aux5,*aux6;
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		aux2=ReadToken(scanner);
		aux3=ReadToken(scanner);
		aux4=ReadToken(scanner);
		aux5=ReadToken(scanner);
		aux6=ReadToken(scanner); /* pasar a int */
		
		switch( UserRegistration(aux1, aux2, aux3, aux4, aux5, atoi(aux6) ) ) {
				
			case REG_PASSWD_ERROR:
				printf("La clave de seguridad debe coincidir con la primer clave ingresada\n");
				break;
			case REG_USER_EXISTS:
				printf("El nombre de usuario no se encuentra disponible\n");
				break;
			case REG_ERROR:
				printf("Se produjo un error al intentar agregar el nuevo usuario\n");
				break;
			case REG_OK:
				printf("Se ha registrado con exito\n");
				break;
			default:
				printf("Se ha producido un error al intentar conectarse al servidor\n");
		}
		
		free(aux1);
		free(aux2);
		free(aux3);
		free(aux4);
		free(aux5);
		free(aux6);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}

static int Logout_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    if(!MoreTokensExist(scanner)) {
		
		switch( UserLogout() ) {
				
			case LOG_OUT_ERROR:
				printf("Se produjo un error al intentar desloguearse\n");
				break;
			case LOG_OUT_ACCES_DENY:
				printf("El usuario ha sido deslogueado desde otra terminal\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
				break;
			case LOG_OUT_USER_NOT_LOG:
				printf("Debe estar logueado para poder desloguearse...\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
				break;
			case LOG_OUT_OK:
				printf("Se ha deslogueado con exito\n");
				strcpy(user, "anonimo");
				break;
			default:
				printf("Se ha producido un error al intentar conectarse al servidor\n");
		}		
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}

static int Download_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1;
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		
		switch( UserDownload(aux1) ) {
				
			case DOWNLOAD_ERROR:
				printf("Se produjo un error al intentar descargar la pelicula\n");
				break;
			case DOWNLOAD_OK:
				printf("Agregada a la seccion de descargas\n");
				break;
			case DOWNLOAD_USER_NOT_LOG:
				printf("Debe estar logueado para realizar descargas\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
				break;
				
			default:
				printf("Se ha producido un error al intentar conectarse al servidor\n");
		}	
		free(aux1);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}

static int BuyMovie_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1,*aux2,*aux3,*aux4;
	char ticket[20];
	
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		aux2=ReadToken(scanner);
		aux3=ReadToken(scanner);
		aux4=ReadToken(scanner);
		
		switch( UserBuyMovie(aux1,aux2,aux3,aux4,ticket) ) {
				
			case BUY_ERROR:
				printf("Se produjo un error al intentar comprar la pelicula\n");
				break;
			case BUY_OK:
				printf("Ticket generado:%s\n",ticket);
				break;
			case BUY_ACCES_DENY:
				printf("El usuario ha sido deslogueado desde otra terminal\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
				break;
			case BUY_USER_NOT_LOG:
				printf("Debe estar logueado para comprar peliculas\n");
				if( strcmp(user, "anonimo") != 0 )
					strcpy(user, "anonimo");
				break;
			case BUY_INVALID_MOVIE:
				printf("La pelicula es inexistente\n");
				break;		
			default:
				printf("Se ha producido un error al efectuar la compra\n");
		}	
		free(aux1);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}


static int ShowCommands(scannerADT scanner, void * data)
{

    printf("Comandos disponibles:\n");
	printf("login user password\n");
	printf("new user password rep_password mail description level\n");
	printf("password newpassword rep_newpassword\n");
	printf("download ticket\n");
	printf("buy movie_name pay_server_name pay_server_user pay_server_password\n");
	printf("exit\n");
	printf("logout\n");

    return OK;
}

/* Se cargan los comandos en el arbol de expresiones
*/

static void LoadTree(treeADT tree)
{
    InsertExpression(tree, "login",  Login_Command);
	InsertExpression(tree, "new",  NewAccount_Command);
	InsertExpression(tree, "password",  ChangePassword_Command);
	InsertExpression(tree, "buy",  BuyMovie_Command);
	InsertExpression(tree, "download",  Download_Command);
	InsertExpression(tree, "logout",  Logout_Command);
    InsertExpression(tree, "help",   ShowCommands);
	InsertExpression(tree, "exit",   Exit_Command);
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
        printf("%s@user:~ $ ",user);
        strAux = ReadLine( stdin );
        if(strcmp(strAux, "\n") != 0)
            status = ReadExpression( tree, strAux, NULL );
        else
            status = 0;
        if(status < 0 )
            fprintf(stderr, "Comando invalido.\n");
		if( status == _EXIT_ ) {
			terminar = TRUE;
		}
        free(strAux);
    }
    FreeTree(tree);
  /*  GoodBye();*/
}
