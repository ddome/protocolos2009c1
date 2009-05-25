/*
 *  main.c
 *  MovieStoreLookup
 *
 *  Created by Damian Dome on 5/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "lookup.h"

int
main(void)
{
	if( InitLookup() == FATAL_ERROR ) {
		fprintf(stderr, "Error al inicializar el servidor\n");
		return 1;
	}
	
	if( StartLookup() == FATAL_ERROR ) {
		fprintf(stderr, "Error durante la ejecución del servidor\n");
		EndLookup();
		return 1;
	}
	
	EndLookup();
	return 0;
}

