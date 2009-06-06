/*
 *  counter.h
 *  MovieStoreServer
 *
 *  Created by Damian Dome on 6/5/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __COUNTER_H__
#define  __COUNTER_H__

typedef unsigned int COUNTER; 

COUNTER LoadCounter(char *path);

int SaveCounter(COUNTER counter,char *path);

#endif