#include"movieDB.h"
#include"listADT.h"

struct dbCDT{
    unsigned int cantGenre;
    unsigned int cantMovies;
    listADT genreList;
};

typedef struct genreNode{
    char genre[MAX_MOVIE_GEN];
    unsigned int cantMoviesGenre;
    listADT movieList;
}genreNode;

typedef struct movieNode{
    movie_t movie;
    char * pathName;
}movieNode;

/*
 *FUNCIONES ESTATICAS
 */


static int
GenreCompare(genreNode * genre1,genreNode* genre2)
{
    int ret;
    ret=strcmp(genre1->genre,genre2->genre);
    return ret;
}

static void
FreeGenre(genreNode * genre)
{
    FreeList(genre->movieList);
    free(genre);
    return;
}

static int
MovieCompare(movieNode * movie1,movieNode * movie2)
{
    return strcmp(movie1->movie.name,movie2->movie.name);
}

static void
MovieFree(movieNode * movie)
{
    free(movie->pathName);
    free(movie);
}

static genreNode *
GetGenreNode(dbADT db ,const char * genre)
{
    genreNode * node;
    int i=0,salgo=0;
    if(listIsEmpty(db->genreList))
	return NULL;
    SetBegin(db->genreList);
    while(!salgo)
    {
	if(GetData(db->genreList,(listElementT*)&node)==0 || i==db->cantGenre)
	    salgo=1;
	else
	{
	    if(strcmp(node->genre,genre)==0)
		return node;
	    i++;
	}
    }
    return NULL;
}

static int
InsertNewMovie(dbADT db, movieNode * newMovie)
{
    genreNode * genre;
    
    if(db==NULL || newMovie==NULL)
	return ERROR;
    if( (genre=GetGenreNode(db,newMovie->movie.gen))==NULL )
    {
	if(NewGenre(db,newMovie->movie.gen)==ERROR)
	    return ERROR;
	genre=GetGenreNode(db,newMovie->movie.gen);
    }
    
    Insert(genre->movieList,newMovie);
    db->cantMovies++;
    genre->cantMoviesGenre++;
    return OK;
}



/*
 *FUNCIONES PUBLICAS
 */

dbADT
NewDB(void)
{
    dbADT ret;
    if( (ret=malloc(sizeof(struct dbCDT)))==NULL )
	return NULL;
    ret->cantGenre=0;
    ret->cantMovies=0;
    if( ( ret->genreList=Newlist( (int(*)(listElementT,listElementT)) GenreCompare, (void(*)(listElementT)) FreeGenre) )==NULL )
    {
	free(ret);
	return NULL;
    }
    return ret;
}

int
InsertMovie(dbADT db,movie_t * movie,const char * pathName)
{
    movieNode * newMovie;
    int len;
    
    if( db==NULL || movie==NULL || pathName==NULL )
	return ERROR;
    
    if( (newMovie=calloc(1,sizeof(struct movieNode)))==NULL )
	return ERROR;
    
    memcpy( &(newMovie->movie),movie,sizeof(movie_t));
    len=strlen(pathName);
    if( (newMovie->pathName=calloc(1,(len+1)*sizeof(char)))==NULL )
    {
	free(newMovie);
	return ERROR;
    }
    strncpy(newMovie->pathName,pathName,len);
    return InsertNewMovie(db,newMovie);
}

movie_t **
GetMoviesByGenre(dbADT db,const char * genre)
{
    movie_t ** resp;
    genreNode * node;
    movieNode * movieN;
    int i=0;
    if( genre==NULL || (node=GetGenreNode(db,genre))==0 )
		return NULL;
    
    if( (resp=calloc(1,sizeof(movie_t *)*(node->cantMoviesGenre+1)))==NULL )
		return NULL;
    if( listIsEmpty(node->movieList) )
		return NULL;
    SetBegin(node->movieList);
    
    while( GetData(node->movieList,(listElementT *)&movieN)==1 && i<node->cantMoviesGenre )
    {
	if( ( resp[i]=calloc(1,sizeof(movie_t)) )==NULL )
	{
	    //Liberar
	    return NULL;
	}
	memcpy(resp[i],&(movieN->movie),sizeof(movie_t));
	i++;
    }
    return resp;
}

int
NewGenre(dbADT db,const char * genreName)
{
    genreNode * genre;
    if(db==NULL || genreName==NULL)
	return ERROR;
    
    if( (genre=calloc(1,sizeof(struct genreNode)))==NULL )
	return ERROR;
    
    genre->cantMoviesGenre=0;
    strcpy(genre->genre,genreName);
    if( (genre->movieList=Newlist( (int(*)(listElementT,listElementT))MovieCompare , (void(*)(listElementT))MovieFree ))==NULL )
    {
	free(genre);
	return ERROR;
    }
    if(Insert(db->genreList,genre))
    {
	db->cantGenre++;
	return OK;
    }
    free(genre);
    return ERROR;
}

char **
ListGenre(dbADT db)
{
    char ** resp;
    int i=0,j;
    genreNode * node;
    if( db==NULL || listIsEmpty(db->genreList) )
	return NULL;
    
    if( (resp=calloc(1,sizeof(char*)*(db->cantGenre+1)))==NULL )
	return NULL;
    
    SetBegin(db->genreList);
    while(GetData(db->genreList,(listElementT*)&node) && i<db->cantGenre)
    {
	if( (resp[i]=calloc(1,sizeof(char)*(MAX_MOVIE_GEN+1)))==NULL )
	{
	    for(j=0;j<=i;j++)
		free(resp[i]);
	    free(resp);
	    return NULL;
	}
	strncpy(resp[i],node->genre,MAX_MOVIE_GEN);
	i++;
    }
    
    return resp;
}

int
GetGenreNum(dbADT db)
{
    if(db==NULL)
	return -1;
    return db->cantGenre;
}

char *
GetMoviePathName(dbADT db,const char * movieName)
{
    genreNode * genreN;
    movieNode * movieN;
    char * resp; 
    int i=0,j=0;
    if(db==NULL || movieName==NULL)
	return NULL;
    if( listIsEmpty(db->genreList) )
	return NULL;
    SetBegin(db->genreList);
    while( GetData(db->genreList,(listElementT*)&genreN)!=0 && i<db->cantGenre)
    {
	if( !listIsEmpty(genreN->movieList) )
	{
	    SetBegin(genreN->movieList);
	    while( GetData(genreN->movieList,(listElementT*)&movieN) && j<genreN->cantMoviesGenre)
	    {
			if( strcmp(movieN->movie.name,movieName)==0 )
			{
				if( (resp=malloc(sizeof(char)*(strlen(movieN->pathName)+1)))==NULL )
				return NULL;
				strcpy(resp,movieN->pathName);
				return resp;
			}
		j++;
	    }
		j=0;
	}
	i++;
    }
    return NULL;
}

unsigned long
GetMoviesNumber(movie_t **movies)
{
	unsigned long num=0;
	while( movies[num] != NULL) {
		num++;		
	}
	return num;
}

void
FreeMovieList(movie_t **movies)
{
	while (*movies != NULL) {
		free(*movies);
		movies++;
	}
}

void
FreeDB(dbADT db)
{
    FreeList(db->genreList);
    free(db);
    return;
}












