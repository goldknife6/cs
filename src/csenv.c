#include "csenv.h"
#include "csutil.h"
#include "cstype.h"
#include "csframe.h"

static void e_free_(void *key,void *val)
{

}

static void e_buildin_(csS_table vtab,csG_string name,csT_type retty,csT_type formalsty,int offset)
{
	VERIFY(name);VERIFY(vtab);
	VERIFY(retty);VERIFY(formalsty);
	csS_symbol s = csS_mksymbol(name);
	csE_enventry foo = csU_malloc(sizeof(*foo));
	csT_typelist formals = csT_mktypelist();
	csT_typelistadd(formals,formalsty);

	foo->kind = csE_fun;
	foo->u.fun.buildin = TRUE;
	foo->u.fun.res = retty;
	foo->u.fun.formals = formals;
	foo->name = s;
	foo->u.fun.frame = csF_buildin_frame(offset);
	csS_insert(vtab,s,foo);
} 

csS_table csE_baseval(void)
{
	csS_table foo = csS_empty(e_free_);
	e_buildin_(foo,"_buildin_printint_",csT_typevoid(),csT_typeint(),1);
	e_buildin_(foo,"_buildin_printstring_",csT_typevoid(),csT_typestring(),2);
	e_buildin_(foo,"_buildin_openfile_",csT_typefile(),csT_typestring(),3);
	e_buildin_(foo,"_buildin_readfile_",csT_typestring(),csT_typefile(),4);
	return foo;
}

csS_table csE_basetype(void)
{
	csS_table foo = csS_empty(e_free_);
	csS_insert(foo,csS_mksymbol("int"), csT_typeint());
	csS_insert(foo,csS_mksymbol("bool"), csT_typebool());
	csS_insert(foo,csS_mksymbol("string"), csT_typestring());
	csS_insert(foo,csS_mksymbol("void"), csT_typevoid());
	csS_insert(foo,csS_mksymbol("file"), csT_typefile());
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
	foo->u.fun.buildin = FALSE;
	foo->u.fun.res = res;
	foo->u.fun.formals = formals;
	foo->name = name;
	foo->u.fun.frame = frame;
	return foo;
}

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
