#include "csobject.h"
#include <stdio.h>

csO_object csF_file(char *filename)
{
	VERIFY(filename);
	FILE *f = fopen(filename,"r+");
	if (!f) return NULL;
	csO_object foo = csO_empty_object();
	foo->kind = csO_file_;
	foo->u.fval.f = f;
	return foo;
}