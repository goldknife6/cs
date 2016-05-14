#include "csobject.h"
#include "cslist.h"
#include <stdlib.h>
#include <string.h>

static void o_initheader_(csO_gcheader *h)
{
	VERIFY(h);
	INIT_LIST_HEAD(&h->next);
	h->marked = FALSE;
}

static csO_string o_ostring_(char *src,int size)
{
	csO_string foo = malloc(sizeof(*foo));
	foo->s = malloc(size + 1);
	foo->s[size] = 0;
	strncpy(foo->s,src,size);
	foo->size = size;
	return foo;
}

csO_object csO_int_object(int val)
{
	csO_object foo = malloc(sizeof(*foo));
	o_initheader_(&foo->header);
	foo->kind = csO_int;
	foo->u.ival = val;
	return foo;
}

csO_object csO_bool_object(csG_bool val)
{
	csO_object foo = malloc(sizeof(*foo));
	o_initheader_(&foo->header);
	foo->kind = csO_bool;
	foo->u.bval = val;
	return foo;
}

csO_object csO_string_object(char * val,size_t size)
{
	csO_object foo = malloc(sizeof(*foo));
	o_initheader_(&foo->header);
	foo->kind = csO_str;
	foo->u.sval = o_ostring_(val,size);
	return foo;
}


