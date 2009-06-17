#ifndef __MOVIE_DB__
#define __MOVIE_DB__

#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include"listADT.h"
#include"../Common/app.h"
#include"../Common/defines.h"

typedef struct dbCDT * dbADT;

dbADT NewDB(void);

int InsertMovie(dbADT db,movie_t * movie,const char * pathName);

movie_t ** GetMoviesByGenre(dbADT db,const char * genre);

int NewGenre(dbADT db,const char * genreName);

char ** ListGenre(dbADT db);

int GetGenreNum(dbADT db);

char * GetMoviePathName(dbADT db,const char * movieName);

unsigned long GetMoviesNumber(movie_t **movies);

void FreeMovieList(movie_t **movies);

void FreeDB(dbADT db);

#endif



