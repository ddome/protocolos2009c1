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
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

#define CONNECT_ERROR -10

#define TIMEOUT_DEFAULT 10

#define TIMEOUT_ERROR -20

#define SERVER_CONFIG "client.config"

typedef enum client_login_status { LOGIN_USER_IS_LOG=-4,LOGIN_CONNECT_ERROR=-3,LOGIN_USER_INVALID =-2, LOGIN_PASS_INVALID=-1, USER_LOGIN_OK=1 } client_login_status;

typedef enum client_change_passwd_status { CHANGE_ACCESS_DENY=-5,CHANGE_LOG_ERROR=-4,NEW_PASSWD_INVALID=-3,CHANGE_CONNECT_ERROR=-2,CHANGE_ERROR=-1,CHANGE_OK=1 } client_change_passwd_status;

typedef enum client_user_reg { REG_PASSWD_ERROR=-4, REG_USER_EXISTS=-3,REG_CONNECT_ERROR=-2,REG_ERROR=-1,REG_OK=1 } client_user_reg;

typedef enum client_logout_status { LOG_OUT_CONNECT_ERROR=-4, LOG_OUT_USER_NOT_LOG=-3,LOG_OUT_ACCES_DENY=-2,LOG_OUT_ERROR=-1,LOG_OUT_OK=1 } client_logout_status;

typedef enum client_download_status { DOWNLOAD_CONNECT_ERROR=-4, DOWNLOAD_USER_NOT_LOG=-3,DOWNLOAD_ACCES_DENY=-2,DOWNLOAD_ERROR=-1,DOWNLOAD_OK=1 } client_download_status;

typedef enum client_buy_movie_status {
	BUY_MOVIE_SERVER_ERROR = -10,
    BUY_MOVIE_USER_ERROR = -9,BUY_MOVIE_NO_CASH=-8,BUY_INVALID_MOVIE=-7,BUY_PASS_ERROR=-6,
    BUY_USER_ERROR=-5,BUY_CONNECT_ERROR=-4,BUY_USER_NOT_LOG=-3,BUY_ACCES_DENY=-2,
    BUY_ERROR=-1,BUY_OK=1 
} client_buy_movie_status;

typedef enum client_list_movies_by_gen_status { LIST_ERROR=-1, LIST_OK=1 } client_list_movies_by_gen_status;

typedef enum list_users_status { LIST_USERS_NOT_LOG=-2,LIST_USERS_ERROR=-1, LIST_USERS_OK=1 } list_users_status;

typedef enum list_gens_t { LIST_GENS_ERROR=-1, LIST_GENS_OK=1 } list_gens_t;


status InitClient(char *host,char *port);
status StartClient(void);
void EndClient(void);

status InitDownloader(void);

client_login_status UserLogin(char *user, char* passwd);

client_change_passwd_status UserChangePasswd(char *new_passwd, char *rep_new_passwd);

client_user_reg UserRegistration(char *user, char *passwd, char *rep_passwd, char *mail, char *desc, int level);

client_logout_status UserLogout(void);

client_download_status UserDownload(char * ticket);

client_buy_movie_status UserBuyMovie(char *movie_name,char *pay_name,char *pay_user, char *pay_passwd, char *ticket_ret);

client_list_movies_by_gen_status ListMoviesByGen(char *gen,movie_t ***movie_list_ptr);

list_users_status ListUsers(client_t ***users_list_ptr);

list_gens_t ListGens(list_movie_request_t *** gens_ptr);

void UserExit(void);

#endif
