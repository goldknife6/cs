#include "csparser.h"
#include "csutil.h"
#include "cslex.h"

static csL_token token;

static csG_bool 		match(csL_tokkind expected,char *msg);
static csA_declist 		p_declist_(void);
static csA_dec 			p_dec_(void);
static csA_paramlist 	p_paramlist_(void);
static csA_param		p_param_(void);
static csA_mutable 		p_mutable_(void);
static csA_immutable 	p_immutable_(void);
static csA_factor 		p_factor_(void);
static csA_uexpr 		p_unaryexpr_(void);
static csA_term 		p_term_(void);
static csA_termlist 	p_termlist_(void);
static csA_urelexpr 	p_urelexpr_(void);
static csA_relexpr 		p_relexpr_(void);
static csA_sumexpr 		p_sumexpr_(void);
static csA_sumexprlist 	p_sumexprlist_(void);
static csA_andexpr 		p_andexpr_(void);
static csA_andlist 		p_andlist_(void);
static csA_simpleexpr 	p_simpleexpr_(void);
static csA_simplelist 	p_simplelist_(void);
static csA_locdec 		p_locvardec_(void);
static csA_locdeclist 	p_locvardeclist_(void);
static csA_expr 		p_expr_(void);
static csA_exprlist 	p_exprlist_(void);
static csA_stmt  		p_stmt_(void);
static csA_stmtlist 	p_stmtlist_(void);
static csA_arglist 		p_arglist_(void);

#define MATCH(tok,msg,lab) ({if (!match(tok,msg)) goto lab;})

static csG_bool match(csL_tokkind expected,char *msg)
{ 
	VERIFY(msg);
	if (token.kind == expected) {
		token = csL_gettoken();
		return TRUE;
	} else {
		csG_pos pos = token.pos;
		csU_emsg("%d:%d:%s\n",pos.row,pos.col,msg);	
		token = csL_gettoken();
		csG_error = TRUE;
		return FALSE;
	}
}

csA_declist parser(void)
{
	token = csL_gettoken();
	return p_declist_();
}

static csA_declist p_declist_(void)
{
	csA_declist foo = NULL;
	csA_dec bar = NULL;
loop:
	switch(token.kind) {
	case csL_DEF:
	case csL_VAR:
		if (!foo) foo = csA_mkdeclist();
		bar = p_dec_();
		if (bar) csA_declistadd(foo,bar);
		goto loop;
	case csL_ENDFILE:
		break;
	default:
		VERIFY(0);
	}
	return foo;
}

static csA_dec p_dec_(void)
{
	csA_dec foo = csA_mkdec();
	csG_pos pos = token.pos;
	if (token.kind == csL_VAR) {
		//enter variable declaration
		MATCH(csL_VAR,"missing var",sync);
		if (token.kind == csL_ID) {		//eat type-id
			csG_string name = csL_tokenstr(token);
			csA_setdecvartype(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing type-id",sync);
		if (token.kind == csL_ID) {		//eat type-id
			csG_string name = csL_tokenstr(token);
			csA_setdecvarname(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing id",sync);
		if (token.kind == csL_ASSIGN) { //simpleexpr is optional
			MATCH(csL_ASSIGN,"missing =",sync);
			csA_simplelist list = p_simplelist_();
			VERIFY(list);
			csA_setdecvarlist(foo,list);
		}
		MATCH(csL_SEMICOLON,"missing ;",sync);
	} else if (token.kind == csL_DEF) {
		MATCH(csL_DEF,"missing def",sync);
		if (token.kind == csL_ID) {
			csG_string name = csL_tokenstr(token);
			csA_setdecfunname(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing function id",sync);
		MATCH(csL_LPAREN,"missing (",sync);
		csA_paramlist p = p_paramlist_();
		
		csA_setdecfunparamlist(foo,p);
		//SET_FUNDEC_RESID(foo,CSSymbol(token.u.sval));
		MATCH(csL_RPAREN,"missing )",sync);
		if (token.kind == csL_ID) {
			csG_string name = csL_tokenstr(token);
			csA_setdecfunrestype(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing type-id",sync);
		MATCH(csL_LBRACE,"missing {",sync);
		csA_locdeclist list = p_locvardeclist_();
		if (list) csA_setdecfunloclist(foo,list);
		csA_stmtlist stmt = p_stmtlist_();
		if (stmt) csA_setdecfunstmtlist(stmt,foo);
		MATCH(csL_RBRACE,"missing }",sync);
	}
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_paramlist p_paramlist_(void)
{
	csA_paramlist foo = NULL;
	csA_param bar = NULL;
loop:
	switch (token.kind) {
	case csL_ID:
		bar = p_param_();
		if (bar) {
			if (!foo) foo = csA_mkparamlist();
			csA_paramlistadd(foo,bar);
			if (token.kind == csL_COMMA) {
				MATCH(csL_COMMA,"missing ,",sync);
				if (token.kind != csL_ID)
					goto sync;
				goto loop;
			}
		} else {
			VERIFY(0);
		}
		break;
	case csL_RPAREN:
		break;
	default:
		VERIFY(0);
	}
	return foo;
sync:
	VERIFY(0);
}

static csA_param p_param_(void)
{
	csA_param foo = csA_mkparam();
	csG_pos pos = token.pos;
	if (token.kind == csL_ID) {
		csG_string name = csL_tokenstr(token);
		csA_setparamtype(foo,csS_mksymbol(name));
	}
	MATCH(csL_ID,"missing type-id",sync);
	if (token.kind == csL_ID) {
		csG_string name = csL_tokenstr(token);
		csA_setparamname(foo,csS_mksymbol(name));
	}
	MATCH(csL_ID,"missing id",sync);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_exprlist p_exprlist_(void)
{
	csA_exprlist foo = NULL;
	csA_expr bar = NULL;
loop:
	switch(token.kind) {
	case csL_NOT:case csL_MINUS:case csL_LPAREN:
	case csL_LBRACK:case csL_NUM:case csL_CHAR:
	case csL_TRUE:case csL_FALSE:case csL_STRING:
	case csL_ID:case csL_DOLLAR:
		if (!foo) foo = csA_mkexprlist();
		bar = p_expr_();
		if (bar) csA_exprlistadd(foo,bar);
		if (token.kind == csL_COMMA) {
			MATCH(csL_COMMA,"missing ,",sync);
			goto loop;
		}
		break;
	case csL_SEMICOLON://FOLLOW
	case csL_RPAREN:
	case csL_RBRACK:
		break;
	default:
		VERIFY(0);
	}
	return foo;
sync:
	VERIFY(0);
}

static csA_expr p_expr_(void)
{
	csA_expr foo = NULL;
	csA_simplelist bar = NULL;
	csA_mutable mut = NULL;
	csA_expr e = NULL;
	csG_pos pos = token.pos;
	switch(token.kind) {
	case csL_NOT:case csL_MINUS:case csL_LPAREN:
	case csL_LBRACK:case csL_NUM:case csL_CHAR:
	case csL_TRUE:case csL_FALSE:case csL_STRING:
	case csL_ID:
		if (!foo) foo = csA_mkexpr();
		bar = p_simplelist_();
		VERIFY(bar);
		csA_setexprlist(foo,bar);
		break;
	case csL_DOLLAR:
		if (!foo) foo = csA_mkexpr();
		MATCH(csL_DOLLAR,"missing $",sync);
		mut = p_mutable_();
		csA_setexprasgnmut(foo,mut);
		MATCH(csL_ASSIGN,"missing =",sync);
		e = p_expr_();
		VERIFY(e);
		csA_setexprasgnexpr(foo,e);
		break;
	default:
		VERIFY(0);
	}
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_mutable p_mutable_(void)
{
	csA_mutable foo = NULL;
	csG_pos pos = token.pos;
	if (token.kind == csL_ID) {
		foo = csA_mkmut();
		csG_string name = csL_tokenstr(token);
		csA_setmutid(foo,csS_mksymbol(name));
	}
	MATCH(csL_ID,"missing mutable",sync);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_immutable p_immutable_(void)
{
	csA_immutable foo = csA_mkimmut();
	csG_pos pos = token.pos;
	switch (token.kind) {
	case csL_NUM:
		csA_setimmutnum(foo,csL_tokennum(token));
		MATCH(csL_NUM,"missing numconst",sync);
		break;
	case csL_CHAR:
		csA_setimmutchar(foo,csL_tokenchar(token));
		MATCH(csL_CHAR,"missing charconst",sync);
		break;
	case csL_STRING:
		csA_setimmutstr(foo,csL_tokenstr(token));
		MATCH(csL_STRING,"missing strconst",sync);
		break;
	case csL_TRUE:
		csA_setimmutbool(foo,TRUE);
		MATCH(csL_TRUE,"missing boolconst",sync);
		break;
	case csL_FALSE:
		csA_setimmutbool(foo,FALSE);
		MATCH(csL_FALSE,"missing boolconst",sync);
		break;
	case csL_LBRACK://function call
		MATCH(csL_LBRACK,"missing [",sync);
		csA_setimmutcallargs(foo,p_arglist_());
		MATCH(csL_RBRACK,"missing ]",sync);
		if (token.kind == csL_ID) {
			csG_string name = csL_tokenstr(token);
			csA_setimmutcallid(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing function-id",sync);
		break;
	default:
		VERIFY(0);
	}
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_arglist p_arglist_(void)
{
	return p_exprlist_();
}

static csA_factor p_factor_(void)
{
	csA_factor foo = csA_mkfactor();
	csA_mutable mut = NULL;
	csA_immutable immut = NULL;
	csG_pos pos = token.pos;
	switch (token.kind) {
	case csL_ID:
		mut = p_mutable_();
		VERIFY(mut);
		csA_setfactormut(foo,mut);
		break;
	case csL_LPAREN:case csL_LBRACK:
	case csL_NUM:case csL_CHAR:case csL_STRING:
	case csL_TRUE:case csL_FALSE:
		immut = p_immutable_();
		VERIFY(immut);
		csA_setfactorimmut(foo,immut);
		break;
	default:
		csL_prttoken(token);
		VERIFY(0);
	}
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_uexpr p_unaryexpr_(void)
{
	csA_uexpr foo = csA_mkuexpr();
	csG_pos pos = token.pos;
	if (token.kind == csL_MINUS) {
		MATCH(csL_MINUS,"missing -",sync);
		foo->flags = TRUE;
	}

	csA_factor fac = p_factor_();
	VERIFY(fac);
	csA_setuexpr(foo,fac);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_term p_term_(void)
{	
	csG_pos pos = token.pos;
	csA_term foo = csA_mkterm();
	csA_uexpr bar = p_unaryexpr_();
	VERIFY(bar);
	csA_settermuexpr(foo,bar);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_termlist p_termlist_(void)
{
	csA_termlist foo = NULL;
	csA_term bar = NULL;
	csG_pos pos ;
loop:
	switch (token.kind) {
	case csL_MINUS:case csL_LPAREN:case csL_LBRACK:
	case csL_NUM:case csL_CHAR:case csL_STRING:
	case csL_TRUE:case csL_FALSE:case csL_ID:
		if (!foo) foo = csA_mktermlist();
		pos = token.pos;
		bar = p_term_();
		VERIFY(bar);
		bar->pos;
		csA_termlistadd(foo,bar);
		switch (token.kind) {
		case csL_TIMES:	
			csA_settermop(bar,csA_times);
			break;
		case csL_DIVIDE:
			csA_settermop(bar,csA_divide);
			break;
		default:
			
			return foo;
		}
		MATCH(token.kind,"missing mulop",sync);
		goto loop;
		break;
	default:
		goto sync;
	}
	return foo;
sync:
	VERIFY(0);
}

static csA_urelexpr p_urelexpr_(void)
{
	csA_urelexpr foo = csA_mkurelexpr();
	csA_relexpr bar = NULL;
	csG_pos pos = token.pos;
	if(token.kind == csL_NOT) {
		MATCH(csL_NOT,"missing !",sync);
		foo->flags = TRUE;
	}
	bar = p_relexpr_();
	VERIFY(bar);
	csA_seturelexprrel(foo,bar);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_relexpr p_relexpr_(void)
{
	csG_pos pos = token.pos;
	csA_relexpr foo = csA_mkrelexpr();
	csA_sumexprlist sum1 = p_sumexprlist_();
	csA_sumexprlist sum2 = NULL;
	csA_op op;
	VERIFY(sum1);
	csA_setrelexprsum1(foo,sum1);
	switch (token.kind) {
	case csL_EQ: 
		csA_setrelexprop(foo,csA_eq);break;
	case csL_NEQ:
		csA_setrelexprop(foo,csA_neq);break;
	case csL_LT: 
		csA_setrelexprop(foo,csA_lt);break;
	case csL_LE:
		csA_setrelexprop(foo,csA_lq);break;
	case csL_GT: 
		csA_setrelexprop(foo,csA_gt);break;
	case csL_GE:
		csA_setrelexprop(foo,csA_gq);break;
	default:
		foo->pos = pos;
		return foo;
	}

	MATCH(token.kind,"missiing rel op",sync);
	sum2 = p_sumexprlist_();
	if (!sum2) goto sync;
	csA_setrelexprsum2(foo,sum2);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_sumexpr p_sumexpr_(void)
{
	csG_pos pos = token.pos;
	csA_sumexpr foo = csA_mksumexpr();
	csA_termlist bar = p_termlist_();
	VERIFY(bar);
	csA_setsumexprterm(foo,bar);
	foo->pos = pos;
	return foo;
sync:
	VERIFY(0);
}

static csA_sumexprlist p_sumexprlist_(void)
{
	csA_sumexprlist foo = NULL;
	csA_sumexpr bar = NULL;
loop:
	switch (token.kind) {
	case csL_MINUS:case csL_LPAREN:case csL_LBRACK:
	case csL_NUM:case csL_CHAR:case csL_STRING:
	case csL_TRUE:case csL_FALSE:case csL_ID:
		if (!foo) foo = csA_mksumexprlist();
		bar = p_sumexpr_();
		VERIFY(bar);
		csA_sumexprlistadd(foo,bar);
		switch (token.kind) {
		case csL_PLUS:	
			csA_setsumexprop(bar,csA_plus);
			break;
		case csL_MINUS:
			csA_setsumexprop(bar,csA_minus);
			break;
		default:
			return foo;
		}
		MATCH(token.kind,"missing sumop",sync);
		goto loop;
	default:
		goto sync;
	}
sync:
	VERIFY(0);
}

csA_andexpr p_andexpr_(void)
{
	csA_andexpr foo = csA_mkandexpr();
	csA_urelexpr bar = p_urelexpr_();
	VERIFY(bar);
	csA_setandexprurel(foo,bar);
	return foo;
}

csA_andlist p_andlist_(void)
{
	csA_andlist foo = NULL;
	csA_andexpr bar = NULL;
	csG_pos pos;
loop:
	switch (token.kind) {
	case csL_NOT:case csL_MINUS:case csL_LPAREN:
	case csL_LBRACK:case csL_NUM:case csL_CHAR:
	case csL_TRUE:case csL_FALSE:case csL_STRING:
	case csL_ID:
		if (!foo) foo = csA_mkandlist();
		pos = token.pos;
		bar = p_andexpr_();
		if(bar) csA_andlistadd(foo,bar);
		else VERIFY(0);
		bar->pos = pos;
		switch (token.kind) {
		case csL_AND:
			MATCH(csL_AND,"missing &",sync);
			break;
		default:
			return foo;
		}
		goto loop;
	default:
		goto sync;	
	}
sync:
	VERIFY(0);
}

csA_simpleexpr p_simpleexpr_(void)
{
	csA_simpleexpr foo = csA_mksimpleexpr();
	csG_pos pos = token.pos;
	csA_andlist bar = p_andlist_();
	VERIFY(bar);
	csA_setsimpleexprand(foo,bar);
	foo->pos = pos;
	return foo;
}

csA_simplelist p_simplelist_(void)
{
	csA_simplelist foo = NULL;
	csA_simpleexpr bar = NULL;
	csG_pos pos;
loop:
	switch (token.kind) {
	case csL_NOT:case csL_MINUS:case csL_LPAREN:
	case csL_LBRACK:case csL_NUM:case csL_CHAR:
	case csL_TRUE:case csL_FALSE:case csL_STRING:
	case csL_ID:
		if (!foo) foo = csA_mksimplelist();
		pos = token.pos;
		bar = p_simpleexpr_();
		if(bar) csA_simplelistadd(foo,bar);
		else VERIFY(0);
		bar->pos = pos;
		switch (token.kind) {
		case csL_OR:
			MATCH(csL_OR,"missing |",sync);
			break;
		default:
			return foo;
		}
		goto loop;
	default:
		goto sync;	
	}
sync:
	VERIFY(0);
}

static csA_locdec p_locvardec_(void)
{
	csA_locdec foo = csA_mklocdec();
	csG_pos pos = token.pos;
	if (token.kind == csL_VAR) {
		MATCH(csL_VAR,"missing var",sync); //eat var keyword
		if (token.kind == csL_ID) {		//eat type-id
			csG_string name = csL_tokenstr(token);
			csA_setlocdectype(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing id",sync);
		if (token.kind == csL_ID) {		//eat id
			csG_string name = csL_tokenstr(token);
			csA_setlocdecname(foo,csS_mksymbol(name));
		}
		MATCH(csL_ID,"missing id",sync);
		if (token.kind == csL_ASSIGN) { //simpleExpr is optional
			MATCH(csL_ASSIGN,"missing =",sync);
			csA_simplelist list = p_simplelist_();
			VERIFY(list);
			csA_setlocdecsimlist(foo,list);
		}
		MATCH(csL_SEMICOLON,"missing ;",sync);
		foo->pos = pos;
		return foo;
	} 
sync:
	VERIFY(0);
}

static csA_locdeclist p_locvardeclist_(void)
{
	csA_locdeclist foo = NULL;
	csA_locdec bar = NULL;
loop:
	switch(token.kind) {
	case csL_VAR:
		if (!foo) foo = csA_mklocdeclist();
		bar = p_locvardec_();
		if (bar) csA_locvardecadd(foo,bar);
		else VERIFY(0);
		goto loop;
	default:
		return foo;
	}
	return foo;
}

static csA_stmtlist p_stmtlist_(void)
{
	csA_stmtlist head = NULL;
	while (TRUE) {
		if (!head) head = csA_mkstmtlist();
		csA_stmt bar = p_stmt_();
		if (!bar) break;
		csA_stmtlistadd(head,bar);
	}
	return head;
}

static csA_stmt p_stmt_(void)
{
	csA_stmt foo = NULL;
	csG_pos pos = token.pos;
	switch(token.kind) {
	case csL_DOLLAR:case csL_NOT:case csL_MINUS:
	case csL_LPAREN:case csL_LBRACK:case csL_NUM:
	case csL_CHAR:case csL_TRUE:case csL_FALSE:
	case csL_STRING:case csL_ID: {
		foo = csA_mkstmt();
		foo->pos = pos;
		csA_exprlist list = p_exprlist_();
		foo->kind = csA_exprstmt;
		foo->u.exprList = list;
		MATCH(csL_SEMICOLON,"missing ;",sync);
		break;
	}
	case csL_SEMICOLON:/*empty stmt*/
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_exprstmt;
		foo->u.exprList = NULL;
		MATCH(csL_SEMICOLON,"missing ;",sync);
		break;
	case csL_IF:{/*ifStmt → if ( exprList ) stmt [ else stmt ]*/
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_ifstmt;
		MATCH(csL_IF,"missing if",sync);
		MATCH(csL_LPAREN,"missing (",sync);
		csA_exprlist list = p_exprlist_();
		VERIFY(list);
		foo->u.ifstmt.list = list;
		MATCH(csL_RPAREN,"missing )",sync);
		foo->u.ifstmt.ifs = p_stmt_();
		if (token.kind == csL_ELSE) {
			MATCH(csL_ELSE,"missing else",sync);
			foo->u.ifstmt.elses = p_stmt_();
		}
		break;
	}
	case csL_FOR:{//struct { csA_exprlist list1,list2,list3;csA_stmt stmt;} forstmt;
		csA_exprlist list = NULL;
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_forstmt;
		MATCH(csL_FOR,"missing for",sync);
		MATCH(csL_LPAREN,"missing (",sync);
		list = p_exprlist_();
		foo->u.forstmt.list1 = list;
		MATCH(csL_SEMICOLON,"missing ;",sync);
		list = p_exprlist_();
		foo->u.forstmt.list2 = list;
		MATCH(csL_SEMICOLON,"missing ;",sync);
		list = p_exprlist_();
		foo->u.forstmt.list3 = list;
		MATCH(csL_RPAREN,"missing )",sync);
		csA_stmt stmt = p_stmt_();
		VERIFY(stmt);
		foo->u.forstmt.stmt = stmt;
		break;
	}
	case csL_WHILE: {
		MATCH(csL_WHILE,"missing while",sync);
		foo = csA_mkstmt();
		foo->pos = pos;
		MATCH(csL_LPAREN,"missing (",sync);
		foo->kind = csA_whilestmt;
		csA_exprlist list = p_exprlist_();
		VERIFY(list);
		foo->u.whestmt.list = list;
		MATCH(csL_RPAREN,"missing )",sync);
		csA_stmt stmt = p_stmt_();
		VERIFY(stmt);
		foo->u.whestmt.stmt = stmt;
		break;
	}
	case csL_LBRACE:{
		MATCH(csL_LBRACE,"missing {",sync);
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_compoundstmt;
		csA_locdeclist var = p_locvardeclist_();
		csA_stmtlist stmt = p_stmtlist_();
		foo->u.comstmt.varlist = var;
		foo->u.comstmt.stmtlist = stmt;
		MATCH(csL_RBRACE,"missing }",sync);
		break;
	}
	case csL_RETURN:
		MATCH(csL_RETURN,"missing return",sync);
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_returnstmt;
		foo->u.retstmt.list = p_exprlist_();
		MATCH(csL_SEMICOLON,"missing ;",sync);
		break;
	case csL_BREAK:
		foo = csA_mkstmt();
		foo->pos = pos;
		foo->kind = csA_breakstmt;
		MATCH(csL_BREAK,"missing break",sync);
		MATCH(csL_SEMICOLON,"missing ;",sync);
		break;
	default:
		return NULL;
	}

	return foo;
sync:
	VERIFY(0);
}