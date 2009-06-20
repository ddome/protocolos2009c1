/*
*  Archivo: payment.c
*  ------------------
*  Codigo principal del paymentServer.
*/

/* Project Includes
*/

#include "payment.h"

/* Server Variables
*/

hashADT psDatabase;
int passive_s;
int transaction_id;

/* Static Functions
*/

static char * GetInvalidRequestStr(void);
static char * GetInvalidClientStr(void);
static char * GetInsuficientCashStr(void);
static char * GetTransactionStr(void);
static int GetNextTransactionId(void);
static void InitMessage(void);
static void ExitMessage(void);

/* Functions
*/

/*
*  Funcion: InitPaymentserver()
*  ----------------------------
*  Inicializa el paymentServer, configurando e iniciando la transmision
*  por protocolo TCP.
*/

status 
InitPaymentServer(void)
{
	int ret;
    char ip[MAXIP];
    char port[MAXPORT];
    FILE * config;
    address_array_t address;
    /* Abrir archivo de configuracion
    */
    if((config = fopen(PAYMENT_CONFIG, "r+")) == NULL)
    {
        fprintf(stderr, "No se pudo abrir el archivo de configuracion.\n");
        return FATAL_ERROR;
    }
    /* Obtener ip y puerto del archivo de configuracion
    */
    if(!GetAddresses(config, &address) || address.count != 1)
    {
        fprintf(stderr, "Archivo de configuracion invalido o corrupto\n");
    }
    
	/* Iniciar TCP 
    */
	if( (passive_s=prepareTCP(address.addresses[0].ip, 
                     address.addresses[0].port, prepareServer)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",passive_s);
		return FATAL_ERROR;
	}	
	if( (ret=listenTCP(passive_s,10)) < 0 ) {
		fprintf(stderr,"No pudo establecerse el puerto para la conexion, retCode=(%d)\n",ret);
		return FATAL_ERROR;
	}
    
    /* Inicializar Base de Datos
    */
    psDatabase =  LoadHashTable(PAYMENT_DB, sizeof(psClient_t), 
                                psComp, psHash, psSave, psLoad);
    if(psDatabase == NULL)
    {
        return FATAL_ERROR;
    }
        
	transaction_id = 0;
	return OK;
}

/*
*  Funcion: StartPaymentserver()
*  ----------------------------
*  Lanza la atencion de pedidos del paymentServer. Para el uso de esta funcion
*  el usuario ya tiene que haber invocado a la funcion InitPaymentServer().
*/


status 
StartPaymentServer(void)
{
	int ssock;
	void * data;
	fd_set rfds;
	fd_set afds;	
	int fd, nfds;
	
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(passive_s,&afds);
    InitMessage();
	while(1) {
		
		memcpy(&rfds, &afds, sizeof(rfds));
		
		if( select(nfds, &rfds, NULL, NULL,NULL) < 0 ) {
			fprintf(stderr, "select: %s\n", strerror(errno));
			return FATAL_ERROR;
		}
		
		/* Si es una nueva conexion la agrego */
		if (FD_ISSET(passive_s, &rfds)) {
						
			if( (ssock=acceptTCP(passive_s)) <= 0 ) {
				printf("Fallo acceptTCP() - retCode=(%d)\n",ssock);
				return FATAL_ERROR;
			}
			
			FD_SET(ssock, &afds);
		}
		
		/* Atiendo cada pedido */
		for(fd=0; fd<nfds; ++fd) {
			if (fd != passive_s && FD_ISSET(fd, &rfds)) {
								
				data=receiveTCP(fd);
				/* Proceso el paquete */
				if( Session(data,fd) != FATAL_ERROR ) {
					close(fd); // Tengo que cerrar la conexion?
					FD_CLR(fd, &afds);
				}
				else {
					return FATAL_ERROR;
				}
			}
		}
	}	
	closeTCP(passive_s);
	
	return OK;
}

/*
*  Funcion: EndPaymentserver()
*  ----------------------------
*  Finaliza el servidor de pagos.
*/

void
EndPaymentServer(void)
{
    FreeHash(psDatabase);
    ExitMessage();
}

/*******************************************************************************/
/*                        Atencion de pedidos                                  */
/*******************************************************************************/

/*
*  Funcion: Session()
*  ------------------
*  Realiza el analisis de los paquetes recibidos y su posterior respuesta.
*/
status
Session(void *data,int socket)
{
	requestPS_t request;
	char reply[MAXREPLY + 1];
	psClient_t client, *clientPtr;
	int status;
	char * ret;
	/* Parseo el texto entrante
	*  Si el request esta mal formado
	*/
	if(!ParsePSRequest((char*)data, &request))
	{
		ret = GetInvalidRequestStr();
		if(ret != NULL)
		{
			strcpy(reply, ret);
			printf("%s", reply);
			printf("\nRequest Mal Formado\n");
			free(ret);
			sendTCP(socket, (void *) reply, strlen(reply) + 1);
		}		
		return ERROR;
	}
	/* Request bien formado, busco al cliente en la db
	*/
	strcpy(client.accountName, request.accountName);
	status = Lookup(psDatabase, (void*)&client);
	
	/* Cliente inexistente o invalido.
	*/
	if(status == -1)
	{
		ret = GetInvalidClientStr();
		if(ret != NULL)
		{
			strcpy(reply, ret);
			printf("%s", reply);
			printf("\nCliente Invalido\n");
			free(ret);
			sendTCP(socket, (void *)reply, strlen(reply)+1);
		}		
		return ERROR;
	}
	clientPtr = GetHElement(psDatabase, status);
	
	/* Dinero insuficiente
	*/
	if(clientPtr->amount - request.amount < EPSILON)
	{
		ret = GetInsuficientCashStr();
		if(ret != NULL)
		{
			strcpy(reply, ret);
			printf("%s", reply);
			printf("\nDinero Insuficiente\n");
			free(ret);
			sendTCP(socket, (void*)reply, strlen(reply)+1);
		}
		return ERROR;
	}
	/* Si todo esta en orden, se realiza el debito
	*/
	ret = GetTransactionStr();
	if(ret == NULL)
	{
		return ERROR;
	}
	clientPtr->amount -= request.amount;
	strcpy(reply, ret);
	printf("%s", reply);
	printf("\nTransaccion OK\n");
	free(ret);
	HDelete(psDatabase, clientPtr);
	HInsert(psDatabase, clientPtr);
	SaveHashTable(psDatabase, PAYMENT_DB);
	sendTCP(socket, (void*)reply, strlen(reply) + 1);
    free(data);
	return OK;
}

static char *
GetInvalidRequestStr(void)
{
	replyPS_t reply;
	reply.statusCode = INVALID_REQUEST_FORMAT;
	strcpy(reply.reply.reason, "Invalid request format.");
	return MakePSReply(reply);
}

static char *
GetInvalidClientStr(void)
{
	replyPS_t reply;
	reply.statusCode = INVALID_ACCOUNT;
	strcpy(reply.reply.reason, "Invalid client.");
	return MakePSReply(reply);
}

static char *
GetInsuficientCashStr(void)
{
	replyPS_t reply;
	reply.statusCode = INSUFICIENT_CASH;
	strcpy(reply.reply.reason, "Insuficient cash in account.");
	return MakePSReply(reply);
}

static char *
GetTransactionStr(void)
{
	replyPS_t reply;
	reply.statusCode = TRANSACTION_SUCCESS;
	reply.reply.transaction = GetNextTransactionId();
	return MakePSReply(reply);
}

static int
GetNextTransactionId(void)
{
	int ret = transaction_id;
	transaction_id = (transaction_id < INT_MAX) ? transaction_id + 1 : 0;
	return ret;
}

static void 
InitMessage(void)
{
    printf("\nInicializando TCP................OK\n");
    printf("Inicializando base de datos........OK\n");
}

static void
ExitMessage(void)
{
    printf("\nLiberando recursos...............OK\n");
}
