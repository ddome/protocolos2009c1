#include"md5.h"

char *
getMD5(const char * pathName)
{
    MD5_CTX md5;
    unsigned char * resp;
    void *readF;
    int size,quit=0,i;
    FILE * file;
    if( (resp=calloc(1,MD5_DIGEST_LENGTH+1))==NULL )
        return NULL;

    if( (readF=calloc(1,READ_CHUNK+1))==NULL )
        return NULL;
    memset(readF,0x0,READ_CHUNK+1);
    
    if( (file=fopen(pathName,"r"))==NULL )
    {
        free(resp);
        return NULL;
    }
    
    //MD5_Init(&md5);
    
    while(!quit)
    {
        size=fread(readF,1,1,file);
        //MD5_Update(&md5,readF,size);
        if(feof(file))
            quit=1;
    }
    
    //MD5_Final(resp,&md5);
    fclose(file);
    
    return (char *)resp;
}


