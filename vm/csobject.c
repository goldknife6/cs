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
	return foo;
}

csO_object csO_string_object_add(csO_object s1,csO_object s2)
{
	static char buffer[1024];
	VERIFY(s1);
	VERIFY(s2);
	VERIFY(s1->kind == csO_string_);
	VERIFY(s2->kind == csO_string_);
	size_t size = s1->u.sval.size + s2->u.sval.size;
	VERIFY(size < 1024);
	sprintf(buffer, "%s%s",s1->u.sval.s,s2->u.sval.s);
	return csS_string(buffer,size);
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


