/*
 *  lookup.h
 *  MovieStoreLookup
 *
 *  Created by Damian Dome on 5/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __LOOKUP_H__
#define  __LOOKUP_H__

#include "../Common/defines.h"
#include "../Common/UDPLib.h"

status  InitLookup(void);
status  StartLookup(void);
void    EndLookup(void);
status Session(void *data,int socket,host_t dest);

#endif
