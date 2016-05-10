#include "csparser.h"
#include "csutil.h"

static void p_declist_(csA_declist dec);
static void p_dec_(csA_dec dec);
static void p_paramlist_(csA_paramlist list);
static void p_param_(csA_param p);
static void p_immutable_(csA_immutable foo);
static void p_mutable_(csA_mutable foo);
static void p_simpleexpr_(csA_simpleexpr foo);
static void p_simplelist_(csA_simplelist list);
static void p_andlist_(csA_andlist foo);
static void p_andexpr_(csA_andexpr foo);
static void p_urelexpr_(csA_urelexpr foo);
static void p_relexpr_(csA_relexpr foo);
static void p_sumexprlist_(csA_sumexprlist list);
static void p_sumexpr_(csA_sumexpr list);
static void p_term_(csA_term foo);
static void p_termlist_(csA_termlist list);
static void p_uexpr_(csA_uexpr foo);
static void p_factor_(csA_factor foo);
static void p_op_(csA_op foo);
static void p_locdeclist_(csA_locdeclist foo);
static void p_locdec_(csA_locdec foo);
static void p_stmtlist_(csA_stmtlist foo);
static void p_stmt_(csA_stmt foo);
static void p_exprlist_(csA_exprlist foo);
static void p_expr_(csA_expr foo);

void testparser(void)
{
	csA_declist dec = parser();
	p_declist_(dec);
}

static void p_declist_(csA_declist dec)
{
	if (!dec) return;
	csA_dec pos = NULL;
	list_for_each_entry(pos, dec, next) {
		VERIFY(pos);
		p_dec_(pos);
	}

}

static void p_dec_(csA_dec dec)
{
	csS_symbol s = NULL;
	csA_paramlist p = NULL;
	csA_simplelist list = NULL;
	if (!dec) return;
	csG_pos pos = csA_decpos(dec);
	switch (dec->kind) {
	case csA_vardec:
		s = csA_decvartype(dec);
		VERIFY(s);
		fprintf(debugs, "var %s ", csS_name(s));
		s = csA_decvarname(dec);
		VERIFY(s);
		fprintf(debugs, "%s", csS_name(s));
		list = csA_decvarlist(dec);
		if (list) {
			fprintf(debugs, " = ");
			p_simplelist_(list);
		}
		fprintf(debugs, ";\n");
		break;
	case csA_fundec:
		s = csA_decfunname(dec);
		VERIFY(s);
		fprintf(debugs, "def %s ", csS_name(s));
		s = csA_decfunrestype(dec);
		VERIFY(s);
		fprintf(debugs, "(");
		p = csA_decfunparamlist(dec);
		p_paramlist_(p);
		fprintf(debugs, ")");
		fprintf(debugs, " %s\n", csS_name(s));
		fprintf(debugs, "{\n");
		p_locdeclist_(csA_decfunloclist(dec));
		p_stmtlist_(csA_decfunstmtlist(dec));
		fprintf(debugs, "}\n");
		break;
	default:
		VERIFY(0);
	}	
}

static void p_paramlist_(csA_paramlist list)
{
	if (!list) return;
	csA_param pos = NULL;
	char *s = "";
	list_for_each_entry(pos, list, next) {
		fprintf(debugs,"%s", s);
		p_param_(pos);
		s = ",";
	}
}

static void p_param_(csA_param foo)
{
	VERIFY(foo);
	csS_symbol s1 = csA_paramtype(foo);
	csS_symbol s2 = csA_paramname(foo);
	VERIFY(s1);
	VERIFY(s2);
	fprintf(debugs, "%s %s",csS_name(s1),csS_name(s2));
}

static void p_immutable_(csA_immutable foo)
{
	if (!foo) return;
	switch (foo->kind) {
	case csA_num_:
		fprintf(debugs,"%d", csA_immutnum(foo));
		break;
	case csA_char_:
		fprintf(debugs,"%c", csA_immutchar(foo));
		break;
	case csA_str_:
		fprintf(debugs,"%s", csA_immutstr(foo));
		break;
	case csA_bool_:
		fprintf(debugs,"%s", csA_immutbool(foo) ? "true" : "false");
		break;
	default:
		VERIFY(0);
	}
}

static void p_mutable_(csA_mutable foo)
{
	if (!foo) return;
	csS_symbol s = csA_mutid(foo);
	fprintf(debugs,"%s", csS_name(s));
}

static void p_simplelist_(csA_simplelist list)
{
	if (!list) return;
	csA_simpleexpr pos;
	char *s = "";
	list_for_each_entry(pos, list, next) {
		fprintf(debugs,"%s", s);
		p_simpleexpr_(pos);
		s = "|";
	}
}

static void p_simpleexpr_(csA_simpleexpr foo)
{
	if (!foo) return;
	p_andlist_(foo->andlist);
}

static void p_andlist_(csA_andlist list)
{
	if (!list) return;
	csA_andexpr pos;
	char *s = "";
	list_for_each_entry(pos, list, next) {
		fprintf(debugs,"%s", s);
		p_andexpr_(pos);
		s = "&";
	}
}

static void p_andexpr_(csA_andexpr foo)
{
	if (!foo) return;
	p_urelexpr_(foo->urelexpr);
}

static void p_urelexpr_(csA_urelexpr foo)
{
	if (!foo) return;
	if (foo->flags)
		fprintf(debugs,"!");
	p_relexpr_(foo->rel);
}

static void p_relexpr_(csA_relexpr foo)
{
	if (!foo) return;
	p_sumexprlist_(foo->sum1);
	p_op_(foo->op);
	p_sumexprlist_(foo->sum2);
}

static void p_sumexprlist_(csA_sumexprlist list)
{
	if (!list) return;
	csA_sumexpr pos;
	list_for_each_entry(pos, list, next) {
		p_sumexpr_(pos);
	}
	
}

static void p_sumexpr_(csA_sumexpr foo)
{
	if (!foo) return;
	p_termlist_(foo->list);
	p_op_(foo->op);
}

static void p_term_(csA_term foo)
{
	if (!foo) return;
	p_uexpr_(foo->uexpr);
	p_op_(foo->op);
}

static void p_termlist_(csA_termlist list)
{
	if (!list) return;
	csA_term pos;
	list_for_each_entry(pos, list, next) {
		p_term_(pos);
	}
}

static void p_uexpr_(csA_uexpr foo)
{
	if (!foo) return;
	if (foo->flags)
		fprintf(debugs,"-");
	p_factor_(foo->factor);
}

static void p_factor_(csA_factor foo)
{
	if (!foo) return;
	switch (foo->kind) {
	case csA_immut:
		p_immutable_(csA_factorimmut(foo));break;
	case csA_mut:
		p_mutable_(csA_factormut(foo));break;
	default:
		VERIFY(0);
	}
}

static void p_locdeclist_(csA_locdeclist list)
{
	if (!list) return;
	csA_locdec pos;
	list_for_each_entry(pos, list, next) {
		p_locdec_(pos);
	}
	
}

static void p_locdec_(csA_locdec foo)
{
	csS_symbol s;
	if (!foo) return;
	s = csA_locdectype(foo);
	fprintf(debugs,"var %s", csS_name(s));
	s = csA_locdecname(foo);
	fprintf(debugs," %s", csS_name(s));
	csA_simplelist list = csA_locdecsimlist(foo);
	if (list) {
		fprintf(debugs," = ");
		p_simplelist_(list);
	}
	fprintf(debugs,";\n");
}

static void p_op_(csA_op foo)
{
	if (!foo) return;
	switch (foo) {
	case csA_plus:
		fprintf(debugs,"+");break;
	case csA_minus:
		fprintf(debugs,"-");break;
	case csA_times:
		fprintf(debugs,"*");break;
	case csA_divide:
		fprintf(debugs,"/");break;
	case csA_eq:
		fprintf(debugs,"==");break;
	case csA_neq:
		fprintf(debugs,"!=");break;
	case csA_lt:
		fprintf(debugs,"<");break;
	case csA_lq:
		fprintf(debugs,"<=");break;
	case csA_gt:
		fprintf(debugs,">");break;
	case csA_gq:
		fprintf(debugs,">=");break;
	default:
		VERIFY(0);
	}
}

static void p_stmtlist_(csA_stmtlist list)
{
	if (!list) return;
	csA_stmt pos = NULL;
	list_for_each_entry(pos, list, next) {
		p_stmt_(pos);
	}
}
/*
csA_exprstmt ,csA_compoundstmt,csA_ifstmt,
		csA_whilestmt,csA_forstmt,csA_returnstmt,csA_breakstmt
*/
static void p_stmt_(csA_stmt foo)
{
	if (!foo) return;
	switch (foo->kind) {
	case csA_exprstmt:
		p_exprlist_(foo->u.exprList);
		fprintf(debugs, ";\n");
		break;
	case csA_compoundstmt:
		fprintf(debugs, "{\n");
		p_locdeclist_(foo->u.comstmt.varlist);
		p_stmtlist_(foo->u.comstmt.stmtlist);
		fprintf(debugs, "}\n");
		break;
	case csA_ifstmt:
		fprintf(debugs, "if(");
		p_exprlist_(foo->u.ifstmt.list);
		fprintf(debugs, ") ");
		p_stmt_(foo->u.ifstmt.ifs);
		if (foo->u.ifstmt.elses)
			fprintf(debugs, "else ");
		p_stmt_(foo->u.ifstmt.elses);
		break;
	case csA_whilestmt:
		break;
	case csA_forstmt:
		break;
	case csA_returnstmt:
		break;
	case csA_breakstmt:
		break;		
	default:
		VERIFY(0);
	}
}
static void p_expr_(csA_expr foo)
{
	if (!foo) return;
	switch (foo->kind) {
	case csA_sim_:
		p_simplelist_(csA_exprsimplelist(foo));
		break;
	case csA_asgn_:
		fprintf(debugs, "$");
		p_mutable_(csA_exprmut(foo));
		fprintf(debugs, " = ");
		p_expr_(csA_exprexpr(foo));
		break;
	default:
		VERIFY(0);
	}
}
static void p_exprlist_(csA_exprlist list)
{
	if (!list) return;
	csA_expr pos = NULL;
	char *s = "";
	list_for_each_entry(pos, list, next) {
		fprintf(debugs, "%s",s);
		p_expr_(pos);
		s = ",";
	}
}