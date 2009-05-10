#include <stdio.h>
#include "../Common/defines.h"
#include "../Common/app.h"
#include "../Common/TCPLib.h"
#include "client.h"


int main (int argc, const char * argv[]) {

	if( InitClient() == FATAL_ERROR ) {
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
