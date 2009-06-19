#include <stdio.h>
#include "payment.h"

int
main(void)
{
    if(InitPaymentServer() != OK)
    {
        fprintf(stderr, "No es posible iniciar el servidor");
        return 1;
    }
    if(StartPaymentServer() != OK)
    {   
        fprintf(stderr, "No es posible iniciar el servidor");
    }
        
    EndPaymentServer();
    
    return 0;
}
