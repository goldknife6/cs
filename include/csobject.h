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
} *csO_string;

typedef struct csO_object {
  enum {csO_int,csO_bool,csO_str} kind;
  csO_gcheader header;
  union {
  	int ival;
  	csO_string sval;
  	csG_bool bval;
  } u;
} *csO_object;



extern csO_object csO_int_object(int val);
extern csO_object csO_bool_object(csG_bool val);
extern csO_object csO_string_object(char * val,size_t size);
extern void csO_pobject(csO_object obj);
#endif/*!CS_OBJECT_H*/