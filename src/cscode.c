#include "cscode.h"
#include "cssymbol.h"
#include "cslist.h"
#include "csutil.h"
#include <string.h>
csC_fraglist fraglist = LIST_HEAD_INIT(fraglist);
static csC_frag curfrag;

static csC_info c_info_();
static void c_fraglistadd_();
static void c_quadlistadd_(csC_quadlist head,csC_quad quad);
static void c_printfrag(csC_frag frag);
static void c_printquad(csC_quad quad);
static csC_frag c_strfrag_(csF_access access,csC_info inf);

static csC_info c_dec_(csS_table val,csS_table type,csA_dec list);

void csC_printcode()
{
	csC_frag pos = NULL;
	list_for_each_entry(pos, &fraglist, next) {
		c_printfrag(pos);
	}
}

static csC_info c_infoconst_(csT_type ty)
{
	csC_info inf;
	memset(&inf,0,sizeof(csC_info));
	VERIFY(ty);
	switch (ty->kind) {
	case csT_int:
		inf.kind = c_intconst_;
		break;
	case csT_bool:
		inf.kind = c_boolconst_;
		break;
	case csT_string:
		inf.kind = c_strconst_;
		break;	
	default:
		VERIFY(0);
	}
	return inf;
}

static csC_info c_info_()
{
	csC_info inf;
	memset(&inf,0,sizeof(inf));
	return inf;
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

static void c_fraglistadd_()
{
	VERIFY(curfrag);
	list_add_tail(&curfrag->next, &fraglist);
	curfrag = NULL;
}

static void c_printfrag(csC_frag frag)
{
	if (!frag) return;
	csF_access acc = NULL;
	switch (frag->kind) {
	case csC_strfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static string:%s ", frag->u.strv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
		break;
	case csC_intfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static int:%d ", frag->u.intv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
		break;
	case csC_boolfrag:
		acc = frag->access;
		VERIFY(acc);
		fprintf(debugs, "static bool:%d ", frag->u.boolv);
		fprintf(debugs, "%d %d\n",acc->kind,acc->u.offset);
		break;
	case csC_procfrag:
		VERIFY(0);
		break;
	default:
		VERIFY(0);
	}
}

static csC_frag c_frag_(csF_access access,csC_info inf)
{
	csC_frag foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	switch (inf.kind) {
	case c_intconst_:
		foo->kind = csC_intfrag;
		foo->u.intv = inf.u.intconst;
		break;
	case c_strconst_:
		foo->kind = csC_strfrag;
		foo->u.strv = inf.u.strconst;
		break;
	case c_boolconst_:
		foo->kind = csC_boolfrag;
		foo->u.boolv = inf.u.boolconst;
		break;
	default:
		VERIFY(0);
	}
	foo->access = access;
	curfrag = foo;
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
		csF_access access = csF_allocglobal(ty);
		e = csE_varentry(ty,access,name);
		csS_insert(vtab, name, e);
		csA_simplelist list = csA_decvarlist(foo);
		if (list) {
			c_frag_(access, c_infoconst_(ty));
		} else {
			c_frag_(access, c_infoconst_(ty));
		}

		c_fraglistadd_();
		break;
	}
	case csA_fundec:
		break;
	default:
		VERIFY(0);
	}
	return inf;
}
