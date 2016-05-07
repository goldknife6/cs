#include "csenv.h"
#include "csutil.h"

static void e_free_(void *key,void *val)
{

}

csS_table csE_baseval(void)
{
	csS_table foo = csS_empty(e_free_);//CSsymEmpty(freeEnrty);
	return foo;
}

csS_table csE_basetype(void)
{
	csS_table foo = csS_empty(e_free_);
	csS_insert(foo,csS_mksymbol("int"), csT_typeint());
	csS_insert(foo,csS_mksymbol("bool"), csT_typebool());
	csS_insert(foo,csS_mksymbol("string"), csT_typestring());
	csS_insert(foo,csS_mksymbol("void"), csT_typevoid());
	return foo;
}

csE_enventry csE_varentry(csT_type type,csF_access access,csS_symbol name)
{
	VERIFY(type);VERIFY(access);VERIFY(name);
	csE_enventry foo = csU_malloc(sizeof(*foo));
	foo->kind = csE_var;
	foo->u.var.type = type;
	foo->u.var.access = access;
	foo->name = name;
}
csE_enventry csE_funentry(csT_typelist formals,csT_type res,csS_symbol name)
{
	VERIFY(formals);VERIFY(res);VERIFY(name);
	csE_enventry foo = csU_malloc(sizeof(*foo));
	foo->kind = csE_fun;
	foo->u.fun.res = res;
	foo->u.fun.formals = formals;
	foo->name = name;
}

