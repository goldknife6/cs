#include "csparser.h"
#include "csutil.h"
#include "cslex.h"

static csL_token token;

static csG_bool 		match(csL_tokkind expected,char *msg);
static csA_declist 		p_declist_();
static csA_dec 			p_dec_();
static csA_paramlist 	p_paramlist_();
static csA_param		p_param_();


#define MATCH(tok,msg,lab) ({if (!match(tok,msg)) goto lab;})

csA_declist parser()
{
	token = csL_gettoken();
	return p_declist_();
}

static csA_declist p_declist_()
{
	csA_declist foo = NULL;
	csA_dec bar = NULL;
loop:
	switch(token.kind) {
	case csL_DEF:
	case csL_VAR:
		bar = p_dec_();
		if (bar) {
			if (!foo) foo = csA_mkdeclist();
			csA_declistadd(foo,bar);
		}
		goto loop;
	case csL_ENDFILE:
		break;
	default:
		VERIFY(0);
	}
	return foo;
}

static csA_dec p_dec_()
{
	csA_dec foo = csA_mkdec();
	csG_pos pos = token.pos;
	if (token.kind == csL_VAR) {
		//enter variable declaration
		MATCH(csL_VAR,"missing var",sync);
		foo->kind = csA_vardec;
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
			//MATCH(ASSIGN,NULL,sync);
			//SET_VARDEC_SMP(foo,simpleExpr());
			//if (!error) VERIFY(VARDEC_SMP(foo));
			VERIFY(0);
		}
		MATCH(csL_SEMICOLON,"missing ;",sync);
	} else if (token.kind == csL_DEF) {
		MATCH(csL_DEF,"missing def",sync);
		foo->kind = csA_fundec;
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
		MATCH(csL_RBRACE,"missing }",sync);
	}
	return foo;
sync:
	VERIFY(0);
}

static csA_paramlist p_paramlist_()
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

static csA_param p_param_()
{
	csA_param foo = csA_mkparam();
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
	return foo;
sync:
	VERIFY(0);
}

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