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

csO_object csO_empty_object()
{
	csO_object foo = malloc(sizeof(*foo));
	o_initheader_(&foo->header);
	memset(foo,0,sizeof(*foo));
	return foo;
}


void csO_pobject(csO_object obj)
{
	VERIFY(obj);
	switch (obj->kind) {
	case csO_string_:
		fprintf(stdout,"%s", obj->u.sval.s);
		break;
	default:
		VERIFY(0);
	}
}


