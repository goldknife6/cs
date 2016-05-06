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

void csA_setdecfunparamlist(csA_dec foo,csA_paramlist list)
{
	VERIFY(foo);
	foo->u.fundec.list = list;
}

csA_paramlist csA_decfunparamlist(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.fundec.list;
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
	list_add_tail(&bar->next, foo);
}


/**************************************************************/
csA_param csA_mkparam()
{
	csA_param foo = csU_malloc(sizeof(*foo));
	return foo;
}

csS_symbol csA_paramtype(csA_param foo)
{
	VERIFY(foo);
	return foo->type;
}

csS_symbol csA_paramname(csA_param foo)
{
	VERIFY(foo);
	return foo->name;
}

void csA_setparamname(csA_param foo,csS_symbol name)
{
	VERIFY(foo);VERIFY(name);
	foo->name = name;
}

void csA_setparamtype(csA_param foo,csS_symbol type)
{
	VERIFY(foo);VERIFY(type);
	foo->type = type;
}

csA_paramlist csA_mkparamlist(void)
{
	csA_paramlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}


void csA_paramlistadd(csA_paramlist foo,csA_param bar)
{
	VERIFY(foo);VERIFY(bar);
	list_add_tail(&bar->next, foo);
}