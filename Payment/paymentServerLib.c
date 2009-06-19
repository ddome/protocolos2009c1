/*
*  Archivo: paymentServerLib.c
*  ---------------------------
*  Implementacion de la interface paymentServerLib.h
*/

#include "paymentServerLib.h"

static int ValidateField(char * field, int lineNumber, int isRequest);

static int ValidateRequest(char * field, int lineNumber);

static int ValidateReply(char * field, int lineNumber);

static int IsValidCharacter(char c);

static int IsValidOpcode(int opCode);

int ParsePSReply(char * reply, replyPS_t * resp)
{
    char * aux;
    int status, auxInt;
    float auxFloat;
    scannerADT scanner = NewScanner();
    SetScannerString(scanner, reply);

    /* Linea 1 */ 
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REP_LINE1, strlen(REP_LINE1) ) == 0)
            && ValidateField(aux + strlen(REP_LINE1), 1, 0);
    if(status == 0){
        return 0;
    }
    auxInt = atoi(aux + strlen(REP_LINE1));

    resp->statusCode = auxInt;
    free(aux);

    if(resp->statusCode != OPERACION_EXITOSA)
    {
        /* Linea 2 fallida */ 
        if(!MoreTokensExist(scanner)){
            return 0;
        }
        aux = ReadToken(scanner);
        status = (strncmp(aux,REP_LINE2_REASON, strlen(REP_LINE2_REASON) ) == 0)
                && ValidateField(aux + strlen(REP_LINE2_REASON), 3, 0);
        if(status == 0){
            return 0;
        }
        strcpy((resp->reply).reason, aux + strlen(REP_LINE2_REASON));
        free(aux);
    }
    else
    {
        /* Linea 2 exitosa */
        if(!MoreTokensExist(scanner)){
            return 0;
        }
        aux = ReadToken(scanner);
        status = (strncmp(aux,REP_LINE2_TRANSC, strlen(REP_LINE2_TRANSC) ) == 0)
                && ValidateField(aux + strlen(REP_LINE2_TRANSC), 2, 0);
        if(status == 0){
            return 0;
        }
        auxInt = atoi(aux + strlen(REP_LINE2_TRANSC));

        (resp->reply).transaction = auxInt;
        free(aux);
    }
    return 1;
}

char * MakePSReply(replyPS_t reply)
{
    size_t size = 0;
    char * resp = calloc(MAXREPLY, sizeof(char));
    char aux[200] = {0};
    if(resp == NULL){
        return NULL;
    }
    /* Linea 1*/
    strcpy(resp, REP_LINE1);
    size += strlen(REP_LINE1);
    sprintf(aux, "%d", reply.statusCode);
    strncpy(resp+size, aux, strlen(aux));
    size+= strlen(aux);
    strncpy(resp + size, "\n", 1);
    size++;

    if(reply.statusCode == OPERACION_EXITOSA)
    {
        strcpy(resp + size, REP_LINE2_TRANSC);
        size += strlen(REP_LINE2_TRANSC);

        sprintf(aux, "%d", reply.reply.transaction);
        strncpy(resp+size, aux, strlen(aux));
        size+= strlen(aux);

        strncpy(resp + size, "\n", 1);
        size++;
    }
    else
    {
        strcpy(resp + size, REP_LINE2_REASON);
        size += strlen(REP_LINE2_REASON);
        strncpy(resp+size, reply.reply.reason, strlen(reply.reply.reason));
        size += strlen(reply.reply.reason);
        strncpy(resp + size, "\n", 1);
        size++;
    }
    printf("\n%s\n", resp);
    return resp;
}

int
ParsePSRequest(char * req, requestPS_t * resp)
{
    char * aux;
    int status, auxInt;
    float auxFloat;
    scannerADT scanner = NewScanner();
    SetScannerString(scanner, req);
    /* Linea 1 */
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strcmp(aux,REQ_LINE1 ) == 0)?1 :0;
    if(status == 0){
        return 0;
    }
    free(aux);
   
   /* Linea 2 */
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REQ_LINE2, strlen(REQ_LINE2) ) == 0)
            && ValidateField(aux + strlen(REQ_LINE2), 2, 1);
    if(status == 0){
        return 0;
    }
    strcpy(resp->clientServer, aux + strlen(REQ_LINE2));
    free(aux);
    /* Linea 3 */ 
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REQ_LINE3, strlen(REQ_LINE3) ) == 0)
            && ValidateField(aux + strlen(REQ_LINE3), 3, 1);
    if(status == 0){
        return 0;
    }
    strcpy(resp->accountName, aux + strlen(REQ_LINE3));
    free(aux);

    /* Linea 4 */ 
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REQ_LINE4, strlen(REQ_LINE4) ) == 0)
            && ValidateField(aux + strlen(REQ_LINE4), 4, 1);
    if(status == 0){
        return 0;
    }
    auxInt = atoi(aux + strlen(REQ_LINE4));

    resp->accountNumber = auxInt;
    free(aux);

    /* Linea 5 */
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REQ_LINE5, strlen(REQ_LINE5) ) == 0)
            && ValidateField(aux + strlen(REQ_LINE5), 5, 1);
    if(status == 0){
        return 0;
    }
    auxInt = atoi(aux + strlen(REQ_LINE5));
    resp->securityCode = auxInt;
    free(aux);

    /* Linea 6 */ 
    if(!MoreTokensExist(scanner)){
        return 0;
    }
    aux = ReadToken(scanner);
    status = (strncmp(aux,REQ_LINE6, strlen(REQ_LINE6) ) == 0)
            && ValidateField(aux + strlen(REQ_LINE6), 6, 1);
    if(status == 0){
        return 0;
    }
    sscanf(aux + strlen(REQ_LINE6), "%f", &auxFloat);
    resp->amount = auxFloat;
    free(aux);
    return 1;
}

char *
MakePSRequest(requestPS_t req)
{
    size_t size = 0;
    char * resp = calloc(MAXREQUEST, sizeof(char));
    char aux[200] = {0};
    if(resp == NULL){
        return NULL;
    }

    strcpy(resp, REQ_LINE1);
    size += strlen(REQ_LINE1);
    strncpy(resp + size, "\n", 1);
    size++;
    strcpy(resp + size, REQ_LINE2);
    size += strlen(REQ_LINE2);
    strncpy(resp+size, req.clientServer, strlen(req.clientServer));
    size += strlen(req.clientServer);
    strncpy(resp + size, "\n", 1);
    size++;

    strcpy(resp + size, REQ_LINE3);
    size += strlen(REQ_LINE3);
    strncpy(resp+size, req.accountName, strlen(req.accountName));
    size += strlen(req.accountName);
    strncpy(resp + size, "\n", 1);
    size++;

    strcpy(resp + size, REQ_LINE4);
    size += strlen(REQ_LINE4);
    //Validar q sea numero lindo
    sprintf(aux, "%d", req.accountNumber);
    strncpy(resp+size, aux, strlen(aux));
    size+= strlen(aux);

    strncpy(resp + size, "\n", 1);
    size++;

    strcpy(resp + size, REQ_LINE5);
    size += strlen(REQ_LINE5);

    sprintf(aux, "%d", req.securityCode);
    strncpy(resp+size, aux, strlen(aux));
    size+= strlen(aux);

    strncpy(resp + size, "\n", 1);
    size++;

    strcpy(resp + size, REQ_LINE6);
    size += strlen(REQ_LINE6);

    sprintf(aux, "%0.2f", req.amount);
    strncpy(resp+size, aux, strlen(aux));
    size += strlen(aux);
    strncpy(resp + size, "\n", 1);
    size++;

    resp[size] = '\0';

    printf("\n%s\n", resp);
    getchar();
    return resp;

}

static int
ValidateField(char * field, int lineNumber, int isRequest)
{
    errno = 0;
    if(isRequest)
        return ValidateRequest(field, lineNumber);
    else
        return ValidateReply(field, lineNumber);
}

static int
ValidateRequest(char * field, int lineNumber)
{
    int len, i, status = 1, auxInt;
    float auxFloat;
    char * auxString;
    switch(lineNumber)
    {
        case 2:
            len = strlen(field);
            if(len > 0 && len <=20){
                for(i = 0; i < len && status == 1; i++)
                    status = IsValidCharacter(*(field+i));
            }
            else
                status = 0;
            return status;
            break;
        case 3:
            len = strlen(field);
            return (len > 0 && len <=MAXNAME);
            break;
        case 4:case 5:
            auxInt = atoi(field);
            return (auxInt > 0 && errno != ERANGE) ? 1 : 0;
            break;
        case 6:
            status = sscanf(field, "%f", &auxFloat);
            /* Verifico que tenga 2 decimales */
            auxString = strchr(field, '.');
            return (status > 0 && auxFloat > 0.0
                    && auxString != NULL && strlen(auxString + 1) == 2) ? 1 : 0;
            break;
    }
}

static int 
ValidateReply(char * field, int lineNumber)
{
    int len, i, status = 1, auxInt;
    float auxFloat;
    char * auxString;
    switch(lineNumber)
    {
        case 1:
            auxInt = atoi(field);
            return ( IsValidOpcode(auxInt) && errno != ERANGE) ? 1 : 0;
            break;
        // Si fue transaccion OK
        case 2:
            auxInt = atoi(field);
            return (errno != ERANGE) ? 1 : 0;
            break;
        // Si es un error
        case 3:
            len = strlen(field);
            return (len > 0 && len <=MAXREASON) ? 1: 0 ;
            break;

    }
    return 1;
}

static int
IsValidCharacter(char c)
{
    return ((c>='A' && c<='Z') || (c>='a' && c<='z') || (c>='0' && c<='9')) ? 1 : 0;
}

static int
IsValidOpcode(int opCode)
{
    return (opCode == OPERACION_EXITOSA || opCode == REQUEST_MAL_FORMADO 
            || opCode == CUENTA_INVALIDA || opCode == FONDOS_INSUFICIENTES) ? 1:0 ;
}
