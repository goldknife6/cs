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
/**************************************************************/
csA_simpleexpr csA_mksimpleexpr()
{
	csA_simpleexpr foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
/**************************************************************/
csG_pos csA_factorpos(csA_factor foo)
{
	VERIFY(foo);
	return foo->pos;
}
csA_factor csA_mkfactor()
{
	csA_factor foo = csU_malloc(sizeof(*foo));
	return foo;
}
void csA_setfactormut(csA_factor foo,csA_mutable mut)
{
	VERIFY(foo);VERIFY(mut);
	foo->kind = csA_mut;
	foo->u.mut = mut;
}
void csA_setfactorimmut(csA_factor foo,csA_immutable immut)
{
	VERIFY(foo);VERIFY(immut);
	foo->kind = csA_immut;
	foo->u.immut = immut;
}
csA_mutable csA_factormut(csA_factor foo)
{
	VERIFY(foo);
	VERIFY(foo->kind == csA_mut);
	return foo->u.mut;
}
csA_immutable csA_factorimmut(csA_factor foo)
{
	VERIFY(foo);
	VERIFY(foo->kind == csA_immut);
	return foo->u.immut;
}
/**************************************************************/
csS_symbol csA_immutcallid(csA_immutable foo)
{
	VERIFY(foo->kind == csA_call);
	return foo->u.call.id;
}
csA_arglist csA_immutcallargs(csA_immutable foo)
{
	VERIFY(foo->kind == csA_call);
	return foo->u.call.args;
}
csG_bool csA_immutbool(csA_immutable foo)
{
	VERIFY(foo->kind == csA_bool);
	return foo->u.val;
}
int csA_immutchar(csA_immutable foo)
{
	VERIFY(foo->kind == csA_char);
	return foo->u.val;
}
int csA_immutnum(csA_immutable foo)
{
	VERIFY(foo->kind == csA_num);
	return foo->u.val;
}
csG_string csA_immutstr(csA_immutable foo)
{
	VERIFY(foo->kind == csA_str);
	return foo->u.str;
}
csG_pos csA_immutpos(csA_immutable foo)
{
	VERIFY(foo);
	return foo->pos;
}
csA_immutable csA_mkimmut()
{
	csA_immutable foo = csU_malloc(sizeof(*foo));
	return foo;
}
void csA_setimmutstr(csA_immutable foo,csG_string str)
{
	VERIFY(foo);VERIFY(str);
	foo->kind = csA_str;
	foo->u.str = str;
}
void csA_setimmutnum(csA_immutable foo,int num)
{
	VERIFY(foo);
	foo->kind = csA_num;
	foo->u.val = num;
}
void csA_setimmutchar(csA_immutable foo,int c)
{
	VERIFY(foo);
	foo->kind = csA_char;
	foo->u.val = c;
}
void csA_setimmutbool(csA_immutable foo,csG_bool b)
{
	VERIFY(foo);
	foo->kind = csA_bool;
	foo->u.val = b;
}
void csA_setimmutcallargs(csA_immutable foo,csA_arglist args)
{
	VERIFY(foo);
	foo->kind = csA_call;
	foo->u.call.args = args;
}
void csA_setimmutcallid(csA_immutable foo,csS_symbol id)
{
	VERIFY(foo);VERIFY(id);
	foo->kind = csA_call;
	foo->u.call.id = id;
}
/**************************************************************/
csG_pos csA_mutpos(csA_mutable foo)
{
	VERIFY(foo);
	return foo->pos;
}
csA_mutable csA_mkmut()
{
	csA_mutable foo = csU_malloc(sizeof(*foo));
	return foo;
}
void csA_setmutid(csA_mutable foo,csS_symbol id)
{
	VERIFY(foo);VERIFY(id);
	foo->id = id;
}
csS_symbol csA_mutid(csA_mutable foo)
{
	VERIFY(foo);
	return foo->id;
}

/**************************************************************/