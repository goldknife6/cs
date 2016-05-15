#include "csenv.h"
#include "csutil.h"

static csE_enventry e_builtin_funentry_(csS_symbol name,csT_type res);

static void e_free_(void *key,void *val)
{

}

csS_table csE_baseval(void)
{
	csS_table foo = csS_empty(e_free_);
	csS_symbol name = csS_mksymbol("printf");
	csT_type res = csT_typevoid();
	csE_enventry e = e_builtin_funentry_(name,res);
	csS_insert(foo,name,e);
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

static csE_enventry e_builtin_funentry_(csS_symbol name,csT_type res)
{
	csE_enventry foo = csU_malloc(sizeof(*foo));
	foo->name = name;
	foo->u.fun.bulitin = TRUE;
	foo->u.fun.res = res;
	foo->kind = csE_fun;
	return foo;
}


csE_enventry csE_varentry(csT_type type,csF_access access,csS_symbol name)
{
	if (!csG_error) {
		VERIFY(type);VERIFY(access);VERIFY(name);
	}
	csE_enventry foo = csU_malloc(sizeof(*foo));
	foo->kind = csE_var;
	foo->u.var.type = type;
	foo->u.var.access = access;
	foo->name = name;
	return foo;
}
csE_enventry csE_funentry(csT_typelist formals,csT_type res,csS_symbol name,csF_frame frame)
{
	if (!csG_error) {
		VERIFY(formals);VERIFY(res);VERIFY(name);VERIFY(frame);
	}
	csE_enventry foo = csU_malloc(sizeof(*foo));
	foo->kind = csE_fun;
	foo->u.fun.res = res;
	foo->u.fun.formals = formals;
	foo->name = name;
	foo->u.fun.frame = frame;
	return foo;
}
/*
struct e_env_ {
	enum {
		csE_var,csE_fun
	} kind;
	csS_symbol name;
	union {
		struct {
			csT_type type;
			csF_access access;
		} var;
		struct {
			csT_typelist formals;
			csT_type res;
			//csF_frame frame;
		} fun;
	} u;
};
*/
csT_type csE_varty(csE_enventry foo)
{
	VERIFY(foo);
	VERIFY(foo->kind == csE_var);
	return foo->u.var.type;
}
csF_access csE_varaccess(csE_enventry foo)
{
	VERIFY(foo);
	VERIFY(foo->kind == csE_var);
	return foo->u.var.access;
}
