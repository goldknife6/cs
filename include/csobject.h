#ifndef CS_OBJECT_H
#define CS_OBJECT_H
#include "csglobal.h"
#include "cslist.h"

typedef struct csO_gcheader {
  csL_list next;
  csG_bool marked;
} csO_gcheader;

typedef struct {
	char *s;
	size_t size;
} csO_string;

typedef struct csO_object {
  enum {csO_int,csO_bool,csO_str} kind;
  csO_gcheader header;
  union {
  	int ival;
  	csO_string *sval;
  	csG_bool bval;
  } u;
} csO_object;

#endif/*!CS_OBJECT_H*/