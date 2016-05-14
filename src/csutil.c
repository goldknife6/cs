#include "csutil.h"
#include "string.h"
#include <stdarg.h>
#include <stdlib.h>
void csU_emsg(char *message,...)
{
	va_list ap;
	VERIFY(message);
	if (filename) fprintf(errors,"%s:",filename);
	va_start(ap,message);
	vfprintf(errors, message, ap);
	va_end(ap);
}

void *csU_malloc(size_t len) 
{
	VERIFY(len>=0);
	void *p = malloc(len);
	if (!p) emitError("\nRan out of memory!\n");
	memset(p,0,len);
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