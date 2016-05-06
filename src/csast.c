#include "csast.h"
#include "csutil.h"

/**************************************************************/


csG_pos csA_decpos(csA_dec foo)
{
	VERIFY(foo);
	return foo->pos;
}

csA_dec csA_mkdec(void)
{
	csA_dec foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	return foo;
}

void csA_setdecvartype(csA_dec foo,csS_symbol type)
{
	VERIFY(foo);VERIFY(type);
	foo->u.vardec.type = type;
}

csS_symbol csA_decvartype(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.vardec.type;
}

void csA_setdecvarname(csA_dec foo,csS_symbol name)
{
	VERIFY(foo);VERIFY(name);
	foo->u.vardec.name = name;
}

csS_symbol csA_decvarname(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.vardec.name;
}

void csA_setdecfunname(csA_dec foo,csS_symbol name)
{
	VERIFY(foo);VERIFY(name);
	foo->u.fundec.name = name;
}

void csA_setdecfunrestype(csA_dec foo,csS_symbol restype)
{
	VERIFY(foo);VERIFY(restype);
	foo->u.fundec.restype = restype;
}

csS_symbol csA_decfunrestype(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.fundec.restype;
}
csS_symbol csA_decfunname(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.fundec.name;
}
/**************************************************************/

csA_declist csA_mkdeclist(void)
{
	csA_declist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
void csA_declistadd(csA_declist foo,csA_dec bar)
{
	VERIFY(foo);VERIFY(bar);
	list_add(&bar->next, foo);
}


/**************************************************************/
