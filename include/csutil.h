#ifndef CS_UTIL_H
#define CS_UTIL_H
#include "csglobal.h"
extern void csU_emsg(char *message,...);
extern void *		csU_malloc(size_t len);
extern void 		csU_free(void *p);
extern csG_string 	csU_strdup(char *s);
extern csG_bool 	csU_strequal(void *key1,void *key2);
extern unsigned long csU_strhash(void *key);
#endif /*!CS_UTIL_H*/