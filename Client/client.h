/*
 *  client.h
 *  MovieStoreClient
 *
 *  Created by Damian Dome on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "../Common/defines.h"
#include "../Common/TCPLib.h"
#include "../Common/genlib.h"
#include "../Common/app.h"

typedef enum client_login_status { LOGIN_CONNECT_ERROR=-3,LOGIN_USER_INVALID =-2, LOGIN_PASS_INVALID=-1, USER_LOGIN_OK=1 } client_login_status;

status InitClient(void);
status StartClient(void);
void EndClient(void);

client_login_status UserLogin(char *user, char* passwd);

#endif