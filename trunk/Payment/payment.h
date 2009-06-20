/*
*  Archivo: payment.h
*  ------------------
*/

/* System Includes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

/* Project Includes
*/

#include "../Common/genlib.h"
#include "../Common/TCPLib.h"
//#include "../Common/des/include/encrypt.h"
//#include "../Common/fileHandler.h"
#include "../Common/paymentServerLib.h"
#include "../Common/config_parser.h"
#include "payment_db.h"
#include "hashADT.h"

/* Defines
*/
#define PAYMENT_CONFIG "./config.ini"
#define PAYMENT_DB "./payment.csv"
#define HOST_PAYMENT "127.0.0.1"
#define PORT_PAYMENT "1600"
/* Function Declarations
*/

/*
*  Funcion: InitPaymentserver()
*  ----------------------------
*  Inicializa el paymentServer, configurando e iniciando la transmision
*  por protocolo TCP.
*/

status InitPaymentServer(void);

/*
*  Funcion: StartPaymentserver()
*  ----------------------------
*  Lanza la atencion de pedidos del paymentServer. Para el uso de esta funcion
*  el usuario ya tiene que haber invocado a la funcion InitPaymentServer().
*/

status StartPaymentServer(void);

/*
*  Funcion: EndPaymentserver()
*  ----------------------------
*  Finaliza el servidor de pagos.
*/

void EndPaymentServer(void);

/*
*  Funcion: Session()
*  ------------------
*  Realiza el analisis de los paquetes recibidos y su posterior respuesta.
*/
status Session(void *data,int socket);

