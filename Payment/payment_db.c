
#include "payment_db.h"


int
psComp( void * v1, void *v2 )
{
        psClient_t *t1,*t2;
	
	t1 = v1;
	t2 = v2;
	
        return strcmp(t1->accountName, t2->accountName);
}

int
psHash( void *v, int size )
{
	int i;
	psClient_t *c = v;
	int num = 0;
	
	i=0;
	while (c->accountName[i] != '\0' ) {
		num += (c->accountName)[i];
		i++;
	}	
	return  num % size;
}

int
psSave(FILE *fd,void *data)
{	
	psClient_t *c =data;
	fprintf(fd,"%s;%s;%d;%0.2f\n",c->accountName,
            c->accountNumber,c->securityCode, c->amount);
	
	return 0;
}

void *
psLoad(FILE *fd)
{
	psClient_t *c = malloc(sizeof(psClient_t));
	char line[50];
	char *token;

	if( fgets(line,512,fd) == NULL )
		return NULL;

	token = strtok (line,";");
    if( token == NULL )//|| strlen(token) > MAXNAME)
		return NULL;
	strcpy(c->accountName,token);

	token = strtok (NULL,";");
    if( token == NULL )//|| strlen(token) > MAXACCOUNTNUM)
		return NULL;
	strcpy(c->accountNumber,token);

	token = strtok (NULL,";");
	if( token == NULL)// || strlen(token) > 6)
		return NULL;
	c->securityCode = atoi(token);

	token = strtok (NULL,";");
	if( token == NULL)
		return NULL;
        sscanf(token, "%f", &(c->amount));
	
	return (void*)c;
}
