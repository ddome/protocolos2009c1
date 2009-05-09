#include <stdio.h>
#include "../Common/defines.h"
#include "server.h"

int main (int argc, const char * argv[]) {
	
	if( InitServer() == FATAL_ERROR ) {
		fprintf(stderr, "Error al inicializar el servidor\n");
		return 1;
	}
	
	if( StartServer() == FATAL_ERROR ) {
		fprintf(stderr, "Error durante la ejecución del servidor\n");
		EndServer();
		return 1;
	}
	
	EndServer();
	return 0;
}
