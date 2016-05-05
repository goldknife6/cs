#include "csutil.h"
#include "stdlib.h"
#include <string.h>


void *csU_malloc(size_t len) 
{
	VERIFY(len>=0);
	void *p = malloc(len);
	if (!p) emitError("\nRan out of memory!\n");
	return p;
}

void csU_free(void *p)
{
	VERIFY(p);
	free(p);
}

csG_string csU_strdup(char *s)
{
	VERIFY(s);
	csG_string p = csU_malloc(strlen(s)+1);
	strcpy(p,s);
	return p;
}

csG_bool csU_strequal(void *key1,void *key2)
{
	VERIFY(key1);VERIFY(key2);
	return !strcmp(key1,key2);
}

unsigned long csU_strhash(void *key)
{
	unsigned long h = 0;
	for(char *p = key; *p; p++)  
       h = h*65599 + *p;
	return h;
}