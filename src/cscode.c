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
static csC_info c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo);
static csC_info c_simple_(csS_table vtab,csS_table ttab,csA_simpleexpr foo);
static csC_info c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo);
static csC_info c_andexpr_(csS_table vtab,csS_table ttab,csA_andexpr foo);
static csC_info c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo);
static csC_info c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo);
static csC_info c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo);
static csC_info c_sumexpr_(csS_table vtab,csS_table ttab,csA_sumexpr foo);
static csC_info c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo);
static csC_info c_term_(csS_table vtab,csS_table ttab,csA_term foo);
static csC_info c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo);
static csC_info c_factor_(csS_table vtab,csS_table ttab,csA_factor foo);
static csC_info c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo);
static csC_info c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo);


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
			csC_info info = c_simplelist_(vtab,ttab,list);
			c_frag_(access, info);
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
static csC_info c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo)
{
	csC_info inf;
	csA_simpleexpr pos;
	if (!foo) VERIFY(0);
	list_for_each_entry(pos, foo, next) {
		inf = c_simple_(vtab,ttab,pos);
	}
	return inf;
}
/*
struct a_simpleexpr_ {
	csL_list next;
	csA_andlist andlist;
	csG_pos pos;
};
*/
static csC_info c_simple_(csS_table vtab,csS_table ttab,csA_simpleexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	inf = c_andlist_(vtab,ttab,csA_simpleexprand(foo));
	return inf;
}

static csC_info c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo)
{
	csC_info inf;
	csA_andexpr pos;
	if (!foo) VERIFY(0);
	list_for_each_entry(pos, foo, next) {
		inf = c_andexpr_(vtab,ttab,pos);
	}
	return inf;
}
/*
struct a_andexpr_ {
	csL_list next;
	csA_urelexpr urelexpr;
	csG_pos pos;
};
*/
static csC_info c_andexpr_(csS_table vtab,csS_table ttab,csA_andexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	inf = c_urelexpr_(vtab,ttab,csA_andexprurel(foo));
	return inf;
}
/*
struct a_urelexpr_ {
	csL_list next;
	csG_bool flags; 
	csA_relexpr rel;
	csG_pos pos;
};
*/
static csC_info c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	if (foo->flags)
		VERIFY(0);
	inf = c_relexpr_(vtab,ttab,csA_urelexprrel(foo));
	return inf;
}
/*
struct a_relexpr_ {
	csA_sumexprlist sum1;
	csA_op op;
	csA_sumexprlist sum2;
	csG_pos pos;
};
*/
static csC_info c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	inf = c_sumexprlist_(vtab,ttab,csA_relexprsum1(foo));
	VERIFY(csA_relexprop(foo));
	return inf;
}

static csC_info c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo)
{
	csC_info inf;
	csA_sumexpr pos;
	if (!foo) VERIFY(0);
	list_for_each_entry(pos, foo, next) {
		inf = c_sumexpr_(vtab,ttab,pos);
	}
	return inf;
}
/*
struct a_sumexpr_ {
	csL_list next;
	csA_op op;
	csA_termlist list;
	csG_pos pos;
};
*/
static csC_info c_sumexpr_(csS_table vtab,csS_table ttab,csA_sumexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	inf = c_termlist_(vtab,ttab,csA_sumexprterm(foo));
	VERIFY(csA_sumexprop(foo));
	return inf;
}

static csC_info c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo)
{
	csC_info inf;
	csA_term pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		inf = c_term_(vtab,ttab,pos);
	}
	return inf;
}
/*
struct a_term_ {
	csL_list next;
	csA_op op;
	csA_uexpr uexpr;
	csG_pos pos;
}; 
*/
static csC_info c_term_(csS_table vtab,csS_table ttab,csA_term foo)
{
	csC_info inf;
	VERIFY(foo);
	inf = c_uexpr_(vtab,ttab,csA_termuexpr(foo));
	VERIFY(csA_termop(foo));
	return inf;
}
/*
struct a_unaryexpr_ {
	csG_bool flags;
	csA_factor factor;
	csG_pos pos;
};
*/
static csC_info c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	if (foo->flags)
		VERIFY(0);
	inf = c_factor_(vtab,ttab,csA_uexprfac(foo));
	return inf;
}
/*
struct a_factor_ {
	enum {csA_immut,csA_mut} kind;
	csG_pos pos;
	union {
		csA_immutable immut;
		csA_mutable mut;
	} u;
};
*/
static csC_info c_factor_(csS_table vtab,csS_table ttab,csA_factor foo)
{
	csC_info inf;
	VERIFY(foo);
	switch (foo->kind) {
	case csA_immut:
		inf = c_immutable_(vtab,ttab,csA_factorimmut(foo));
		break;
	case csA_mut:
		inf = c_mutable_(vtab,ttab,csA_factormut(foo));
		break;
	default:
		VERIFY(0);
	}
	return inf;
}
/*
struct a_immutable_ {
	csG_pos pos;
	enum {
		csA_expr,csA_call,csA_num,csA_char,
		csA_str,csA_bool
	} kind;
	union {
		int val; 	// NUM CHAR true false
		csG_string str;	// STRING
		//CSastExpr expr;
		/* exprList is optional
		struct {csA_arglist args;csS_symbol id;} call;
	} u;
};
*/
static csC_info c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo)
{
	csC_info inf;
	VERIFY(foo);

	return inf;
}
/*
struct a_mutable_ {
	csG_pos pos;
	csS_symbol id; 
	//CSastExpr expr;
};*/
static csC_info c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo)
{
	csC_info inf;
	VERIFY(foo);

	return inf;
}