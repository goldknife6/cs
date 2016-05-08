#include "cscode.h"
#include "cssymbol.h"
#include "cslist.h"
#include "csutil.h"

csC_fraglist fraglist = LIST_HEAD_INIT(fraglist);
static csC_frag curfrag;

static csC_info c_info_();
static void c_fraglistadd_(csC_frag frag);
static void c_quadlistadd_(csC_quadlist head,csC_quad quad);
static void c_printfrag(csC_frag frag);
static void c_printquad(csC_quad quad);
static csC_frag c_strfrag_(csF_access access,csG_string str);
static csC_frag c_intfrag_(csF_access access,int val);
static csC_frag c_boolfrag_(csF_access access,csG_bool val);

static csC_info c_dec_(csS_table val,csS_table type,csA_dec list);

void csC_printcode()
{
	csC_frag pos = NULL;
	list_for_each_entry_reverse(pos, &fraglist, next) {
		c_printfrag(pos);
	}
}

static csC_info c_info_()
{

}

static void c_quadlistadd_(csC_quadlist head,csC_quad quad)
{
	VERIFY(quad);
	list_add_tail(&quad->next, &head);
}

static void c_printquad(csC_quad quad)
{
	VERIFY(quad);

	switch (quad->kind) {
 	case csC_lable:
 		VERIFY(0);
 	case csC_assign:
 		VERIFY(0);
 	case csC_goto:
 		VERIFY(0);
 	case csC_iffalse:
 		VERIFY(0);
 	case csC_if:
 		VERIFY(0);
 	case csC_add:
 		VERIFY(0);
 	case csC_call:
 		VERIFY(0);
 	case csC_param:
 		VERIFY(0);
 	case csC_minus:
 		VERIFY(0);
 	case csC_multiply:
 		VERIFY(0);
 	case csC_divide:
 		VERIFY(0);
 	case csC_sub:
 		VERIFY(0);
 	default:
		VERIFY(0);
	}
}

static void c_fraglistadd_(csC_frag frag)
{
	VERIFY(frag);
	list_add_tail(&frag->next, &fraglist);
}

static void c_printfrag(csC_frag frag)
{
	if (!frag) return;
	csC_quad pos = NULL;
	switch (frag->kind) {
	case csC_strfrag:/*
		list_for_each_entry(pos, &frag->u.dec, next) {
			c_printquad(pos);
		}*/
		break;
	case csC_intfrag:
		break;
	case csC_boolfrag:
		break;
	case csC_procfrag:
		VERIFY(0);
	default:
		VERIFY(0);
	}
}

static csC_frag c_strfrag_(csF_access access,csG_string str)
{
	csC_frag foo = csU_malloc(sizeof(*foo));
	foo->kind = csC_strfrag;
	foo->u.strv.strv = str;
	foo->u.access = access;
	return foo;
}

static csC_frag c_intfrag_(csF_access access,int val)
{
	csC_frag foo = csU_malloc(sizeof(*foo));
	foo->kind = csC_intfrag;
	foo->u.intv.intv = val;
	foo->u.access = access;
	return foo;
}

static csC_frag c_boolfrag_(csF_access access,csG_bool val)
{
	csC_frag foo = csU_malloc(sizeof(*foo));
	foo->kind = csC_boolfrag;
	foo->u.boolv.boolv = val;
	foo->u.access = access;
	return foo;
}

csC_info c_declist_(csS_table val,csS_table type,csA_declist list)
{
	csC_info inf = c_info_();
	if (!list) return inf;
	csA_dec pos = NULL;
	list_for_each_entry(pos, list, next) {
		VERIFY(pos);
		c_dec_(val,type,pos);
	}
}

static csC_info c_dec_(csS_table vtab,csS_table ttab,csA_dec foo)
{
	VERIFY(foo);
	csC_info inf = c_info_();
	switch (foo->kind) {
	case csA_vardec:{
		csS_symbol tyname = csA_decvartype(foo);
		csS_symbol name = csA_decvarname(foo);
		csT_type ty = csS_look(ttab, tyname);
		VERIFY(ty);
		csE_enventry e = csS_look(vtab, name);
		VERIFY(!e);
		e = csE_varentry(ty,csF_allocglobal(ty),name);
		csS_insert(vtab, name, e);
		//c_decfrag();
		csA_simplelist list = csA_decvarlist(foo);
		if (list) {

		}
		c_fraglistadd_(curfrag);
		break;
	}
	case csA_fundec:
		break;
	default:
		VERIFY(0);
	}
	return inf;
}
