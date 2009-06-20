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
#include "../Common/app.h"

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

static char * AddSpaces(char * token){
	int i;
	for(i = 0; i< strlen(token); i++){
		if( *(token + i) == '#'){
			*(token+i) = ' ';
		}
	}
	return token;
}

/* Comandos del Prompt
*/


static int Exit_Command(scannerADT scanner, void * data)
{
	if(MoreTokensExist(scanner)){
		return _COMMAND_NOT_VALID_;
	}
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
		if(MoreTokensExist(scanner)){
			aux2=ReadToken(scanner);
			if(MoreTokensExist(scanner)){			
				return _COMMAND_NOT_VALID_;
			}

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
			free(aux2);
		}
		else{
			retValue=_COMMAND_NOT_VALID_;
		}	
		free(aux1);
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
		if(MoreTokensExist(scanner)) {
			aux2=ReadToken(scanner);
			if(MoreTokensExist(scanner)){
				return _COMMAND_NOT_VALID_;
			}
		
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
			free(aux2);
		}
		else{
			retValue = _COMMAND_NOT_VALID_;
		}
		free(aux1);
    }
    else {
		retValue=_COMMAND_NOT_VALID_;
    }
	
    return retValue;    
}

static int ListMoviesByGen_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1;
	movie_t **movies_list;
	int i=0;
	
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		if(MoreTokensExist(scanner)){
			return _COMMAND_NOT_VALID_;
		}
		switch( ListMoviesByGen(aux1,&movies_list) ) {
			case LIST_OK:
				

				printf("------------------------------------------------\n");
				printf("  Lista de peliculas de genero %s\n",aux1);
				printf("------------------------------------------------\n");
				while(movies_list[i] != NULL) {
					printf("------------------------------------------------\n");
					printf("                   %s:							\n", movies_list[i]->name);
					printf("------------------------------------------------\n");
					printf("%s\n", movies_list[i]->plot);
					printf("Duracion: %ld minutos \n", movies_list[i]->duration);
					printf("Precio: $%0.2f\n", movies_list[i]->value);
					printf("Size: %ld bytes \n", movies_list[i]->size);
					printf("MD5: %s \n", movies_list[i]->MD5);	
					printf("------------------------------------------------\n\n");
					free(movies_list[i]);					
					i++;
				}
									
				free(movies_list);
				
				break;
			case LIST_ERROR:
				printf("Genero inexistente\n");
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

static int ListGens_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
	list_movie_request_t **gens_list;
	int i=0;
	
    if(!MoreTokensExist(scanner)) {
		
		switch( ListGens(&gens_list) ) {
			case LIST_OK:
				
				printf("------------------------------------------------\n");
				printf("          Lista de generos disponibles		    \n");
				printf("------------------------------------------------\n");
				while(gens_list[i] != NULL) {
					printf("%s\n",(char *)gens_list[i]);
					free(gens_list[i]);					
					i++;
				}
				printf("------------------------------------------------\n\n");
				free(gens_list);
				
				break;
			case LIST_ERROR:
				printf("Genero inexistente\n");
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


static int ListUsers_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
	client_t **users_list;
	int i=0;
	
    if(!MoreTokensExist(scanner)) {
		
		switch( ListUsers(&users_list) ) {
			case LIST_USERS_OK:
				
				printf("------------------------------------------------\n");
				printf("|               Lista de usuarios               |\n");
				printf("------------------------------------------------\n\n\n");
				
				while(users_list[i] != NULL) {
					printf("------------------------------------------------\n");
					printf("                     %s                         \n",users_list[i]->user);
					printf("------------------------------------------------\n");
					printf("%s                                              \n",users_list[i]->desc);
					printf("Contacto: %s:                                   \n",users_list[i]->mail);
					printf("Nivel de usuario: %d                            \n",users_list[i]->level);
					printf("------------------------------------------------\n\n");

					i++;
				}				
				
				break;
			case LIST_USERS_ERROR:
				printf("Error al listar usuarios\n");
				break;
			case LIST_USERS_NOT_LOG:
				if( strcmp(user, "anonimo") != 0 ) {
					printf("Ha sido deslogueado desde otra terminal\n");
					strcpy(user, "anonimo");
				}
				else
					printf("Debe estar logueado para listar usuarios\n");
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

static int NewAccount_Command(scannerADT scanner, void * data)
{
    int retValue = _COMMAND_OK_;
    char * aux1,*aux2,*aux3,*aux4,*aux5,*aux6;
    if(MoreTokensExist(scanner)) {
		aux1=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux2=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux3=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux4=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux5=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux6=ReadToken(scanner); /* pasar a int */
		if(MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		
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
				printf("El ticket ha expirado\n");
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
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux2=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux3=ReadToken(scanner);
		if(!MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		aux4=ReadToken(scanner);
		if(MoreTokensExist(scanner)) {
			return _COMMAND_NOT_VALID_;
		}
		
		switch( UserBuyMovie(aux1,aux2,AddSpaces(aux3),aux4,ticket) ) {
				
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
            case BUY_MOVIE_NO_CASH:
                printf("Fondos insuficientes\n");
                break;
            case BUY_MOVIE_USER_ERROR:
                printf("Cuenta invalida\n");
                break;
			case BUY_MOVIE_SERVER_ERROR:
                printf("Payment server inexistente\n");
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

	printf("------------------------------------------------\n");
    printf("              Comandos disponibles              \n");
	printf("------------------------------------------------\n");
	printf("login user password\n");
	printf("new user password rep_password mail description level\n");
	printf("password newpassword rep_newpassword\n");
	printf("listmovies gen\n");
	printf("listusers\n");
	printf("listgens\n");
	printf("download ticket\n");
	printf("buy movie_name pay_server_name pay_server_user pay_server_account_number\n");
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
	InsertExpression(tree, "listmovies",  ListMoviesByGen_Command);
	InsertExpression(tree, "listusers",  ListUsers_Command);
	InsertExpression(tree, "listgens",  ListGens_Command);
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
