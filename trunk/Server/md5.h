#ifndef __MD5_LIB__
#define __MD5_LIB__

#include <openssl/md5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define READ_CHUNK 512

char * getMD5(const char * pathName);

#endif
