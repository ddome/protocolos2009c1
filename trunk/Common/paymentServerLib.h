/*
*  Archivo: paymentServerLib.h
*  ---------------------------
*  Interface que provee funciones para codificacion y decodificacion
*  de los mensajes utilizados en el protocolo orientado a texto de los
*  payment servers.
*/

/* System Includes
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

/* Project Includes
*/
#include "scannerPS.h"


#ifndef __PAYMENT_SERVER_LIB_H
#define __PAYMENT_SERVER_LIB_H

/* Defines
*/
#define MAX_CLIENT_SERVER 20
#define MAXNAME 63
#define MAXACCOUNTNUM 63
#define MAXREASON 255
#define MAXREQUEST (MAX_CLIENT_SERVER + MAXNAME+100)
#define MAXREPLY   (MAXREASON + 100)

#define REQ_LINE1 "CHARGE PaymentService/1.0"
#define REQ_LINE2 "ClientServer:"
#define REQ_LINE3 "AccountName:"
#define REQ_LINE4 "AccountNumber:"
#define REQ_LINE5 "SecurityCode:"
#define REQ_LINE6 "Amount:"

#define REP_LINE1 "PaymentService/1.0 "
#define REP_LINE2_TRANSC "Transaction:"
#define REP_LINE2_REASON "Reason:"

#define TRANSACTION_SUCCESS    0
#define INVALID_REQUEST_FORMAT 1
#define INVALID_ACCOUNT        2
#define INSUFICIENT_CASH       3


#define EPSILON 0.00001

/* Tipos
*/

/* Tipo: psClient_t
*  ----------------
*  Estructura que caracteriza cada cuenta en la base de
*  datos de un determinado payment server.
*/
typedef struct{
    char accountName[MAXNAME + 1];
    char accountNumber[MAXACCOUNTNUM + 1];
    int securityCode;
    float amount;
} psClient_t;

/* Tipo: StatusCode
*  ----------------
*  Opciones de estados posibles para las respuestas del PS.
*/
typedef enum {OPERACION_EXITOSA,
              REQUEST_MAL_FORMADO,
              CUENTA_INVALIDA,
              FONDOS_INSUFICIENTES} StatusCode;

/* Tipo: requestPS_t
*  -----------------
*  Estructura que contiene un request del protocolo orientado a texto
*  utilizado por los payment servers.
*/
typedef struct{
    char clientServer[MAX_CLIENT_SERVER + 1];
    char accountName[MAXNAME + 1];
    char accountNumber[MAXACCOUNTNUM + 1];
    int securityCode;
    float amount;
} requestPS_t;

/* Tipo: reply_t
*  -------------
*  Union que almacena el numero de transaccion en caso de que sea
*  una respuesta satisfactoria, o un texto que detalla la razon de la 
*  falla en caso de que sea erronea.
*/
typedef union{
    int transaction;
    char reason[MAXREASON + 1];
} reply_t;

/* Tipo: replyPS_t
*  ---------------
*  Guarda una respuesta del protocolo orientado a texto utilizado en los
*  payment servers. El campo statusCode indica el estado, que en caso de 
*  corresponder a OPERACION_EXITOSA, el usuario podra acceder al campo
*  transaction de la union. Para otros valores de statusCode el usuario debera
*  trabajar con el campo reason de la union.
*/
typedef struct{
    StatusCode statusCode;
    reply_t reply;
} replyPS_t;

/* Funciones
*/

/* Funcion: MakePSRequest()
*  ------------------------
*  Recibe una estructura request, y con ella arma el string que 
*  sera enviado para hacer un request
*/
char * MakePSRequest(requestPS_t req);

/* Funcion: ParsePSRequest()
*  -------------------------
*  Recibe un string a parsear, y la direccion de memoria de un request, donde
*  volcara los valores extraidos del string req. Es responsabilidad del usuario
*  alocar memoria para la estructura. Si el valor de retorno es 0, significa que
*  el string req es invalido, por lo tanto los campos de resp no tienen valores seteados.
*  En caso de parseo exitoso devuelve 1.
*/
int ParsePSRequest(char * req, requestPS_t * resp);

/* Funcion: MakePSRequest()
*  ------------------------
*  Recibe una estructura reply, y con ella arma el string que 
*  sera enviado para hacer un reply
*/
char * MakePSReply(replyPS_t reply);

/* Funcion: ParsePSRequest()
*  -------------------------
*  Recibe un string a parsear, y la direccion de memoria de un reply, donde
*  volcara los valores extraidos del string req. Es responsabilidad del usuario
*  alocar memoria para la estructura. Si el valor de retorno es 0, significa que
*  el string req es invalido, por lo tanto los campos de resp no tienen valores seteados.
*  En caso de parseo exitoso devuelve 1.
*/
int ParsePSReply(char * reply, replyPS_t * resp);

#endif

