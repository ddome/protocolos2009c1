#include <stdio.h>
#include "../Common/defines.h"
#include "server.h"

int main (int argc, const char * argv[]) {

	int ret=0;
	
	if( InitServer() == FATAL_ERROR ) {
		fprintf(stderr, "Error al inicializar el servidor\n");
		ret = 1;
	}
	
	if( StartServer() == FATAL_ERROR ) {
		fprintf(stderr, "Error durante la ejecución del servidor\n");
		ret = 1;
	}
	
	EndServer();

	return ret;
}
