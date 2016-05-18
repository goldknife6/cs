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
  enum {csO_string_}kind;
  csO_gcheader header;
  union {
    csO_string sval;
  } u;
} *csO_object;

typedef struct csO_value {
  enum {csO_empty_,csO_addr_,csO_int_,csO_bool_,csO_obj_}kind;
  union {
    int ival;
    int addr;
    csG_bool bval;
    csO_object gc;
  } u;
} csO_value;

//extern csO_object csO_int_object(int val);
//extern csO_object csO_bool_object(csG_bool val);
extern csO_object csO_string_object(char * val,size_t size);
extern void csO_pobject(csO_object obj);
#endif/*!CS_OBJECT_H*/