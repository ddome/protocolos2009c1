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

typedef enum client_login_status { LOGIN_USER_IS_LOG=-4,LOGIN_CONNECT_ERROR=-3,LOGIN_USER_INVALID =-2, LOGIN_PASS_INVALID=-1, USER_LOGIN_OK=1 } client_login_status;

typedef enum client_change_passwd_status { NEW_PASSWD_INVALID=-3,CHANGE_CONNECT_ERROR=-2,CHANGE_ERROR=-1,CHANGE_OK=1 } client_change_passwd_status;

status InitClient(void);
status StartClient(void);
void EndClient(void);

client_login_status UserLogin(char *user, char* passwd);

client_change_passwd_status UserChangePasswd(char* old_passwd, char *new_passwd, char *rep_new_passwd);

#endif