#include <stdio.h>
#include "../Common/defines.h"
#include "../Common/app.h"
#include "../Common/TCPLib.h"
#include "client.h"


int main (int argc, const char * argv[]) {

	char *aux1=NULL,*aux2=NULL;
	if( argc > 1 ) {
		aux1 = argv[1];
	}
	if( argc > 2 ) {
		aux2 = argv[2];
	}	
	
	if( InitClient(aux1,aux2) == FATAL_ERROR ) {
		fprintf(stderr, "Se ha producido un error al inicializar la aplicacion\n");
		return 1;
	}
	if( StartClient() == FATAL_ERROR ) {
		fprintf(stderr, "Se ha producido un error durante la ejecucion de la aplicacion\n");
		return 1;
	}
	
	EndClient();
	
	return 0;
}
