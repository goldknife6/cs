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
	foo->kind = csA_vardec;
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
	foo->kind = csA_vardec;
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
	foo->kind = csA_fundec;
	foo->u.fundec.name = name;
}

void csA_setdecfunrestype(csA_dec foo,csS_symbol restype)
{
	VERIFY(foo);VERIFY(restype);
	foo->kind = csA_fundec;
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
csA_simplelist csA_decvarlist(csA_dec foo)
{
	VERIFY(foo);
	return foo->u.vardec.list;
}
void csA_setdecvarlist(csA_dec foo,csA_simplelist list)
{
	VERIFY(foo);VERIFY(list);
	foo->u.vardec.list = list;
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
csA_simplelist csA_mksimplelist()
{
	csA_simplelist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
void csA_simplelistadd(csA_simplelist foo,csA_simpleexpr bar)
{
	VERIFY(foo);VERIFY(bar);
	list_add_tail(&bar->next, foo);
}
csA_simpleexpr csA_mksimpleexpr()
{
	csA_simpleexpr foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	return foo;
}
csA_andlist csA_simpleexprand(csA_simpleexpr foo)
{
	VERIFY(foo);
	return foo->andlist;
}
void csA_setsimpleexprand(csA_simpleexpr foo,csA_andlist and)
{
	VERIFY(foo);VERIFY(and);
	foo->andlist = and;
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
csG_pos csA_uexprpos(csA_uexpr foo)
{
	VERIFY(foo);
	return foo->pos;
}
void csA_setuexprpos(csA_uexpr foo,csG_pos pos)
{
	VERIFY(foo);
	foo->pos = pos;
}
csA_uexpr csA_mkuexpr()
{
	csA_uexpr foo = csU_malloc(sizeof(*foo));
	foo->flags = FALSE;
	return foo;
}
void csA_setuexpr(csA_uexpr foo,csA_factor factor)
{
	VERIFY(foo);VERIFY(factor);
	foo->factor = factor;
}
csA_factor csA_uexprfac(csA_uexpr foo)
{
	VERIFY(foo);
	return foo->factor;
}
/**************************************************************/
csA_termlist csA_mktermlist(void)
{
	csA_termlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
void csA_termlistadd(csA_termlist foo,csA_term bar)
{
	VERIFY(foo);VERIFY(bar);
	list_add_tail(&bar->next, foo);
}
csG_pos csA_termpos(csA_term foo)
{
	VERIFY(foo);
	return foo->pos;
}
void csA_settermpos(csA_term foo,csG_pos pos)
{
	VERIFY(foo);
	foo->pos = pos;
}
csA_term csA_mkterm()
{
	csA_term foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	return foo;
}
void csA_settermuexpr(csA_term foo,csA_uexpr uexpr)
{
	VERIFY(foo);
	foo->uexpr = uexpr;
}
csA_uexpr csA_termuexpr(csA_term foo)
{
	VERIFY(foo);
	return foo->uexpr;
}
void csA_settermop(csA_term foo,csA_op op)
{
	VERIFY(foo);VERIFY(op);
	switch (op) {
	case csA_times: case csA_divide:
		foo->op = op;
		break;
	default:
		VERIFY(0);
	}
}
csA_op csA_termop(csA_term foo)
{
	VERIFY(foo);
	return foo->op;
}

/**************************************************************/
csG_pos csA_urelexprpos(csA_urelexpr foo)
{
	VERIFY(foo);
	return foo->pos;
}
void csA_seturelexprpos(csA_urelexpr foo,csG_pos pos)
{
	VERIFY(foo);
	foo->pos = pos;
}
csA_urelexpr csA_mkurelexpr()
{
	csA_urelexpr foo = csU_malloc(sizeof(*foo));
	foo->flags = FALSE;
	INIT_LIST_HEAD(&foo->next);
	return foo;
}
csA_relexpr csA_urelexprrel(csA_urelexpr foo)
{
	VERIFY(foo);
	return foo->rel;
}
void csA_seturelexprrel(csA_urelexpr foo,csA_relexpr rel)
{
	VERIFY(foo);VERIFY(rel);
	foo->rel = rel;
}
/**************************************************************/
csA_relexpr csA_mkrelexpr(void)
{
	csA_relexpr foo = csU_malloc(sizeof(*foo));
	return foo;
}
csG_pos csA_relexprpos(csA_relexpr foo)
{
	VERIFY(foo);
	return foo->pos;
}
void csA_setrelexprpos(csA_relexpr foo,csG_pos pos)
{
	VERIFY(foo);
	foo->pos = pos;
}
void csA_setrelexprop(csA_relexpr foo,csA_op op)
{
	VERIFY(foo);
	switch (op) {
	case csA_eq: case csA_neq:
	case csA_lt: case csA_lq:
	case csA_gt: case csA_gq:
		foo->op = op;
		break;
	default:
		VERIFY(0);
	}
	
}
csA_op csA_relexprop(csA_relexpr foo)
{
	VERIFY(foo);
	return foo->op;
}
void csA_setrelexprsum1(csA_relexpr foo,csA_sumexprlist sum)
{
	VERIFY(foo);VERIFY(sum);
	foo->sum1 = sum;
}
csA_sumexprlist csA_relexprsum1(csA_relexpr foo)
{
	VERIFY(foo);
	return foo->sum1;
}
void csA_setrelexprsum2(csA_relexpr foo,csA_sumexprlist sum)
{
	VERIFY(foo);VERIFY(sum);
	foo->sum2 = sum;
}
csA_sumexprlist csA_relexprsum2(csA_relexpr foo)
{
	VERIFY(foo);
	return foo->sum2;
}
/**************************************************************/
csA_sumexprlist csA_mksumexprlist(void)
{
	csA_sumexprlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
void csA_sumexprlistadd(csA_sumexprlist head,csA_sumexpr sum)
{
	VERIFY(head);
	VERIFY(sum);
	list_add_tail(&sum->next, head);
}
csA_sumexpr csA_mksumexpr(void)
{
	csA_sumexpr foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	return foo;
}
csG_pos csA_sumexprpos(csA_sumexpr sum)
{
	VERIFY(sum);
	return sum->pos;
}
void csA_setsumexprpos(csA_sumexpr sum,csG_pos pos)
{
	VERIFY(sum);
	sum->pos = pos;
}
csA_term csA_sumexprterm(csA_sumexpr sum)
{
	VERIFY(sum);
	return sum->term;
}
void csA_setsumexprterm(csA_sumexpr sum,csA_term term)
{
	VERIFY(sum);VERIFY(term);
	sum->term = term;
}
void csA_setsumexprop(csA_sumexpr foo,csA_op op)
{
	VERIFY(foo);
	switch (op) {
	case csA_plus: case csA_minus:
		foo->op = op;
		break;
	default:
		VERIFY(0);
	}
}
csA_op csA_sumexprop(csA_sumexpr foo)
{
	VERIFY(foo);
	return foo->op;
}
/**************************************************************/
csA_andlist csA_mkandlist()
{
	csA_andlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	return foo;
}
void csA_andlistadd(csA_andlist head,csA_andexpr bar)
{
	VERIFY(head);
	VERIFY(bar);
	list_add_tail(&bar->next, head);
}
csA_andexpr csA_mkandexpr()
{
	csA_andexpr foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	return foo;
}
csA_urelexpr csA_andexprurel(csA_andexpr foo)
{
	VERIFY(foo);
	return foo->urelexpr;
}
void csA_setandexprurel(csA_andexpr foo,csA_urelexpr urelexpr)
{
	VERIFY(foo);
	foo->urelexpr = urelexpr;
}
/**************************************************************/