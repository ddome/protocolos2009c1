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

#define CONNECT_ERROR -10

#define HOST_DOWNLOAD "127.0.0.1"
#define HOST_SERVER   "127.0.0.1"

#define PORT_DOWNLOAD "1050"
#define PORT_SERVER   "1044"

typedef enum client_login_status { LOGIN_USER_IS_LOG=-4,LOGIN_CONNECT_ERROR=-3,LOGIN_USER_INVALID =-2, LOGIN_PASS_INVALID=-1, USER_LOGIN_OK=1 } client_login_status;

typedef enum client_change_passwd_status { CHANGE_ACCESS_DENY=-5,CHANGE_LOG_ERROR=-4,NEW_PASSWD_INVALID=-3,CHANGE_CONNECT_ERROR=-2,CHANGE_ERROR=-1,CHANGE_OK=1 } client_change_passwd_status;

typedef enum client_user_reg { REG_PASSWD_ERROR=-4, REG_USER_EXISTS=-3,REG_CONNECT_ERROR=-2,REG_ERROR=-1,REG_OK=1 } client_user_reg;

typedef enum client_logout_status { LOG_OUT_CONNECT_ERROR=-4, LOG_OUT_USER_NOT_LOG=-3,LOG_OUT_ACCES_DENY=-2,LOG_OUT_ERROR=-1,LOG_OUT_OK=1 } client_logout_status;

typedef enum client_download_status { DOWNLOAD_CONNECT_ERROR=-4, DOWNLOAD_USER_NOT_LOG=-3,DOWNLOAD_ACCES_DENY=-2,DOWNLOAD_ERROR=-1,DOWNLOAD_OK=1 } client_download_status;


status InitClient(void);
status StartClient(void);
void EndClient(void);

client_login_status UserLogin(char *user, char* passwd);

client_change_passwd_status UserChangePasswd(char *new_passwd, char *rep_new_passwd);

client_user_reg UserRegistration(char *user, char *passwd, char *rep_passwd, char *mail, char *desc, int level);

client_logout_status UserLogout(void);

client_download_status UserDownload(char * ticket);

void UserExit(void);

#endif