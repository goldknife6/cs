#include "cstype.h"
#include "csutil.h"

csT_typelist csT_mktypelist()
{
	csT_typelist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->head);
	return foo;
}

void csT_typelistadd(csT_typelist head,csT_type type)
{
	VERIFY(head);VERIFY(type);
	t_typelistentry_ foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	foo->type = type;
	list_add_tail(&foo->next, &head->head);
	head->count++;
}

csG_bool CStypeEqual(csT_type foo,csT_type bar)
{
	VERIFY(foo);
	VERIFY(bar);
	if (foo == bar)
		return TRUE;
	return FALSE;
}

csT_type csT_typeint(void) 
{
	/*dynamic allocation,so we can free them*/
	static csT_type foo = NULL;
	if (!foo) {
		foo = csU_malloc(sizeof(*foo));
		foo->kind = csT_int;
	}
	return foo;
}

csT_type csT_typebool(void) 
{
	static csT_type foo = NULL;
	if (!foo) {
		foo = csU_malloc(sizeof(*foo));
		foo->kind = csT_bool;
	}
	return foo;
}

csT_type csT_typestring(void) 
{
	static csT_type foo = NULL;
	if (!foo) {
		foo = csU_malloc(sizeof(*foo));
		foo->kind = csT_string;
	}
	return foo;
}

csT_type csT_typevoid(void) 
{
	static csT_type foo = NULL;
	if (!foo) {
		foo = csU_malloc(sizeof(*foo));
		foo->kind = csT_void;
	}
	return foo;
}