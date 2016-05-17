#include "cscode.h"
#include "cssymbol.h"
#include "cslist.h"
#include "csutil.h"
#include <string.h>
#include <stdarg.h>

typedef struct c_info_ {
	enum {
		c_empty_,c_const_,c_env_,c_var_
	} kind;
	csT_type ty;
	union {
		int intconst;
		csG_string strconst;
		csG_bool boolconst;
		csE_enventry env;
	} u;
} c_info_;

#define INITINF(x) c_info_ x = {c_empty_,NULL,0}

static csC_address c_empty_addr_ = {csC_empty,0};
static csT_type c_returnty_;
static csF_frame c_frame_;
static csC_quadlist c_quad_;
static void c_emsg_(csG_pos pos,char *message,...);

static void c_dec_(csS_table vtab,csS_table ttab,csA_dec foo,csC_fraglist fraglist);
static void c_locdeclist(csS_table vtab,csS_table ttab,csA_locdeclist list);
static c_info_ c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo,csG_bool emit);
static c_info_ c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo,csG_bool emit);
static c_info_ c_andexpr_(csS_table vtab,csS_table ttab,csA_andexpr foo,csG_bool emit);
static c_info_ c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo,csG_bool emit);
static c_info_ c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo,csG_bool emit);
static c_info_ c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo,csG_bool emit);
static c_info_ c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo,csG_bool emit);
static c_info_ c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo,csG_bool emit);
static c_info_ c_factor_(csS_table vtab,csS_table ttab,csA_factor foo,csG_bool emit);
static c_info_ c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo,csG_bool emit);
static c_info_ c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo,csG_bool emit);
static void c_stmtlist_(csS_table vtab,csS_table ttab,csA_stmtlist foo,csT_label lable);
static c_info_ c_exprlist_(csS_table vtab,csS_table ttab,csA_exprlist foo,csG_bool emit);
static c_info_ c_expr_(csS_table vtab,csS_table ttab,csA_expr foo,csG_bool emit);
static c_info_ c_stmt_(csS_table vtab,csS_table ttab,csA_stmt foo,csT_label lable);
static void c_arglist_(csS_table vtab,csS_table ttab,csA_arglist foo);



static void c_frag_(csF_access access,csC_fraglist fraglist,c_info_ inf);
static csC_quadlist c_quadlist_();
static csC_address c_address_int_(int intconst);
static csC_address c_address_string_(csG_string strconst);
static csC_address c_address_bool_(csG_bool boolconst);
static csC_address c_address_env_(csE_enventry eval);
static csC_address c_address_lable_(csT_label lab);
static csC_frag c_procfrag_(csC_quadlist body,csF_frame frame,csC_fraglist fraglist);
static c_info_ c_infoconst_(csT_type ty);
static void c_emitcode_(c_quadkind_ kind,csC_address addr);
static csC_address c_infotoaddr(c_info_ inf);


static csT_typelist c_mktypelist_(csS_table ttab,csA_paramlist foo)
{
	csT_typelist list = csT_mktypelist();
	csA_param pos;
	int i = 0;
	if (!foo) return list;
	list_for_each_entry(pos, foo, next) {
		csS_symbol tyname = csA_paramtype(pos);
		csT_type ty = csS_look(ttab, tyname);
		if(!ty) {
			c_emsg_(pos->pos,"unknown type %s",csS_name(tyname));
			continue;
		}
		csT_typelistadd(list,ty);
		i++;
	}
	list->count = i;
	return list;
}

csC_fraglist c_declist_(csS_table val,csS_table type,csA_declist list)
{
	if (!list) return NULL;
	csC_fraglist fraglist = csU_malloc(sizeof(*fraglist));
	INIT_LIST_HEAD(fraglist);
	csA_dec pos = NULL;
	list_for_each_entry(pos, list, next)
		c_dec_(val,type,pos,fraglist);
	return fraglist;
}
static c_info_ c_consta_(csS_table vtab,csS_table ttab,csA_const c)
{
	INITINF(inf);
	VERIFY(c);
	switch (c->kind) {
	case csA_cnum_:
		inf.u.intconst = c->u.val;
		inf.ty = csT_typeint();		
		break;
	case csA_cchar_:
		VERIFY(0);
	case csA_cstr_:
		inf.u.strconst = c->u.str;
		inf.ty = csT_typestring();	
		break;
	case csA_cbool_:
		inf.u.boolconst = c->u.val;
		inf.ty = csT_typebool();	
		break;
	default:
		VERIFY(0);
	}
	inf.kind = c_const_;
	return inf;
}
static void c_dec_(csS_table vtab,csS_table ttab,csA_dec foo,csC_fraglist fraglist)
{
	VERIFY(foo);
	switch (foo->kind) {
	case csA_vardec: {
		csS_symbol tyname = csA_decvartype(foo);
		csS_symbol name = csA_decvarname(foo);
		csT_type ty = csS_look(ttab, tyname);
		if(!ty) {
			c_emsg_(foo->pos,"unknown type %s",csS_name(tyname));
			return;
		} else if (ty == csT_typevoid()) {
			c_emsg_(foo->pos,"variable %s declared void",csS_name(name));
			return;
		}
		csE_enventry e = csS_look(vtab, name);
		if(e) c_emsg_(foo->pos,"redefinition of variable %s",csS_name(name));
		csF_access access = NULL;
		access = csF_allocstatic();
		e = csE_varentry(ty,access,name);
		csS_insert(vtab, name, e);
		csA_const c = csA_decvarconst(foo);
		if (c) {
			c_info_ info = c_consta_(vtab,ttab,c);
			VERIFY(info.ty);
			if (ty != info.ty) {
				c_emsg_(foo->pos,"incompatible types when assigning");
				return ;
			}
			c_frag_(access,fraglist, info);
		} else {
			c_frag_(access,fraglist, c_infoconst_(ty));
		}
		break;
	}
	case csA_fundec:{
		csS_symbol restype = csA_decfunrestype(foo);
		csS_symbol name = csA_decfunname(foo);
		csT_type ty = csS_look(ttab, restype);
		if(!ty) c_emsg_(foo->pos,"unknown return type %s for function %s",csS_name(restype),csS_name(name));
		c_returnty_ = ty;
		csE_enventry e = csS_look(vtab, name);
		if(e) c_emsg_(foo->pos,"redefinition of function %s",csS_name(name));
		csA_paramlist plist = csA_decfunparamlist(foo);
		csT_typelist list = c_mktypelist_(ttab,plist);
		csF_frame frame = csF_newframe(name);
		c_frame_ = frame;
		e = csE_funentry(list,ty,name,frame);
		csS_insert(vtab, name, e);
		{
			csS_beginscope(vtab);
			csA_param pos = NULL;
			if (plist) {
				list_for_each_entry(pos, plist, next) {
					csF_access access = csF_alloclocal(frame);
					csT_type type = csS_look(ttab, csA_paramtype(pos));
					csS_symbol name = csA_paramname(pos);
					VERIFY(name);
					if (csS_looktop(vtab,name)) {
						c_emsg_(foo->pos,"redefinition of parameter %s",csS_name(name));
					} else {
						csS_insert(vtab, name, csE_varentry(type,access,name));
					}
				}
			}
			c_quadlist_();
			csA_locdeclist list = csA_decfunloclist(foo);
			if (list) c_locdeclist(vtab,ttab,list);
			csA_stmtlist stmt = csA_decfunstmtlist(foo);
			if (stmt)
				c_stmtlist_(vtab,ttab,stmt,NULL);
			csS_endscope(vtab);
		}
		c_procfrag_(c_quad_,frame,fraglist);
		break;
	}
	default:
		VERIFY(0);
	}
}


static void c_locdeclist(csS_table vtab,csS_table ttab,csA_locdeclist list)
{
	VERIFY(list);
	csA_locdec pos = NULL;
	int varcount = 0;
	list_for_each_entry(pos, list, next) {
		csS_symbol tyname = csA_locdectype(pos);
		csS_symbol name = csA_locdecname(pos);
		csT_type ty = csS_look(ttab, tyname);
		if(!ty) {
			c_emsg_(pos->pos,"unknown type %s",csS_name(tyname));
			break;
		} else if (ty == csT_typevoid()) {
			c_emsg_(pos->pos,"variable %s declared void",csS_name(name));
			break;
		}
		csE_enventry e = csS_looktop(vtab, name);
		if(e) c_emsg_(pos->pos,"redefinition of variable %s",csS_name(name));
		csF_access access = csF_alloclocal(c_frame_);
		csA_simplelist list = csA_locdecsimlist(pos);
		e = csE_varentry(ty,access,name);
		
		if (list) {
			c_emitcode_(csC_loadaddr,c_address_env_(e));
			c_info_ tmp = c_simplelist_(vtab,ttab,list,TRUE);
			VERIFY(tmp.ty);
			if (ty != tmp.ty) {
				c_emsg_(pos->pos,"incompatible types when assigning");
				VERIFY(0);
			}
			c_emitcode_(csC_storel,c_empty_addr_);
		} else {
			c_emsg_(pos->pos,"must initialize when assigning");
		}
		
		csS_insert(vtab, name, csE_varentry(ty,access,name));
		varcount++;
	}
}

static c_info_ c_expr_(csS_table vtab,csS_table ttab,csA_expr foo,csG_bool emit)
{
	VERIFY(foo);
	INITINF(inf);
	switch (foo->kind) {
	case csA_sim_:
		VERIFY(csA_exprsimplelist(foo));
		inf = c_simplelist_(vtab,ttab,csA_exprsimplelist(foo),emit);
		break;
	case csA_asgn_: {
		VERIFY(csA_exprmut(foo));
		inf = c_mutable_(vtab,ttab,csA_exprmut(foo),FALSE);
		csT_type mutty = inf.ty;
		VERIFY(mutty);
		csE_enventry e = inf.u.env;
		c_emitcode_(csC_loadaddr,c_address_env_(e));

		VERIFY(csA_exprexpr(foo));
		inf = c_expr_(vtab,ttab,csA_exprexpr(foo),TRUE);
		
		csT_type exprty = inf.ty;
		VERIFY(exprty);
		if (exprty != mutty) 
			c_emsg_(foo->pos,"incompatible types when assigning");
		VERIFY(e->kind == csE_var);
		csF_access access = e->u.var.access;
		if (access->kind == f_static)
			c_emitcode_(csC_stores,c_empty_addr_);
		else if (access->kind == f_frame)
			c_emitcode_(csC_storel,c_empty_addr_);
		else
			VERIFY(0);
		break;
	}
	default:
		VERIFY(0);
	}
	return inf;
}

static c_info_ c_exprlist_(csS_table vtab,csS_table ttab,csA_exprlist foo,csG_bool emit)
{
	INITINF(inf);
	csA_expr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		if (emit && list_is_last(&pos->next,foo))
			inf = c_expr_(vtab,ttab,pos,TRUE);
		else
			inf = c_expr_(vtab,ttab,pos,FALSE);
	}
	return inf;
}

static c_info_ c_stmt_(csS_table vtab,csS_table ttab,csA_stmt pos,csT_label lable)
{
	INITINF(inf);
	VERIFY(pos);
	switch (pos->kind) {
		case csA_exprstmt:
			if (pos->u.exprList)
				c_exprlist_(vtab,ttab,pos->u.exprList,FALSE);
			break;
		case csA_ifstmt:{
			VERIFY(pos->u.ifstmt.list);
			c_info_ inf = c_exprlist_(vtab,ttab,pos->u.ifstmt.list,TRUE);
			csT_label L1 = csT_newlabel();
			c_emitcode_(csC_fjp,c_address_lable_(L1));

			VERIFY(pos->u.ifstmt.ifs);
			inf = c_stmt_(vtab,ttab,pos->u.ifstmt.ifs,lable);

			csA_stmt elses = pos->u.ifstmt.elses;
			csT_label L2 = NULL;
			if (elses) {
				L2 = csT_newlabel();
				c_emitcode_(csC_ujp,c_address_lable_(L2));
			}
			c_emitcode_(csC_lab,c_address_lable_(L1));
			if (elses) {
				inf = c_stmt_(vtab,ttab,elses,lable);
				c_emitcode_(csC_lab,c_address_lable_(L2));
			}
			break;
		}
		case csA_compoundstmt:{
			csA_locdeclist list = pos->u.comstmt.varlist;
			if (list)
				c_locdeclist(vtab,ttab,list);
			csA_stmtlist stmt = pos->u.comstmt.stmtlist;
			if (stmt)
				c_stmtlist_(vtab,ttab,stmt,lable);
			break;
		}
		case csA_whilestmt:{
			csA_exprlist list = pos->u.whestmt.list;
			csA_stmt stmt = pos->u.whestmt.stmt;
			VERIFY(list);VERIFY(stmt);
			csT_label in = csT_newlabel();
			csT_label out = csT_newlabel();
			c_emitcode_(csC_lab,c_address_lable_(in));
			inf = c_exprlist_(vtab,ttab,list,TRUE);
			VERIFY(inf.ty);
			if (inf.ty != csT_typebool()) 
				c_emsg_(pos->pos,"expression type in while statement must be bool");
			c_emitcode_(csC_fjp,c_address_lable_(out));
			inf = c_stmt_(vtab,ttab,stmt,out);
			c_emitcode_(csC_ujp,c_address_lable_(in));
			c_emitcode_(csC_lab,c_address_lable_(out));
			break;
		}
		case csA_forstmt: {
			csA_exprlist list1 = pos->u.forstmt.list1;
			csA_exprlist list2 = pos->u.forstmt.list2;
			csA_exprlist list3 = pos->u.forstmt.list3;
			csA_stmt stmt = pos->u.forstmt.stmt;
			if (list1)
				inf = c_exprlist_(vtab,ttab,list1,FALSE);
			csT_label loopbody = csT_newlabel();
			csT_label out = csT_newlabel();
			
			if (list2) {
				inf = c_exprlist_(vtab,ttab,list2,TRUE);
				VERIFY(inf.ty);
				if (inf.ty != csT_typebool()) 
					c_emsg_(pos->pos,"2th expression type in for statement must be bool");
				c_emitcode_(csC_fjp,c_address_lable_(out));
			}
			c_emitcode_(csC_lab,c_address_lable_(loopbody));
			VERIFY(stmt);
			inf = c_stmt_(vtab,ttab,stmt,out);
			if (list3)
				inf = c_exprlist_(vtab,ttab,list3,TRUE);
			if (list2) {
				inf = c_exprlist_(vtab,ttab,list2,TRUE);
				c_emitcode_(csC_tjp,c_address_lable_(loopbody));
				c_emitcode_(csC_lab,c_address_lable_(out));
			} else {
				c_emitcode_(csC_ujp,c_address_lable_(loopbody));
			}
			break;
		}
		case csA_returnstmt: {
			VERIFY(c_returnty_);
			csA_exprlist list = pos->u.retstmt.list;
			if (list && c_returnty_ == csT_typevoid()) {
				c_emsg_(pos->pos,"return with a value, in function returning void");
			} else if (!list && c_returnty_ != csT_typevoid()) {
				c_emsg_(pos->pos,"with no value, in function returning non-void");
			} else if (list) {
				inf = c_exprlist_(vtab,ttab,list,TRUE);
				if (!csG_error)
					VERIFY(inf.ty);
				if (inf.ty != c_returnty_)
					c_emsg_(pos->pos,"return type is wrong");
			}
			c_emitcode_(csC_ret,c_empty_addr_);
			break;
		}
		case csA_breakstmt: {
			if (!lable)
				c_emsg_(pos->pos,"break statement not whthin for or while statement");
			else
				c_emitcode_(csC_ujp,c_address_lable_(lable));
			break;
		}
		default:
			VERIFY(0);
	}
	return inf;
}

static void c_stmtlist_(csS_table vtab,csS_table ttab,csA_stmtlist foo,csT_label lable)
{
	VERIFY(foo);
	csA_stmt pos;
	list_for_each_entry(pos, foo, next)
		c_stmt_(vtab,ttab,pos,lable);
}

static c_info_ c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo,csG_bool emit)
{
	INITINF(inf);
	csA_simpleexpr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		c_info_ tmp = c_andlist_(vtab,ttab,csA_simpleexprand(pos),emit);
		if (inf.kind != c_empty_) {
			VERIFY(tmp.ty);
			VERIFY(tmp.ty->kind == csT_bool);
			c_emitcode_(csC_or,c_empty_addr_);
		}
		inf = tmp;
	}
	return inf;
}

static c_info_ c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo,csG_bool emit)
{
	INITINF(inf);
	csA_andexpr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		c_info_ tmp = c_urelexpr_(vtab,ttab,csA_andexprurel(pos),emit);
		if(tmp.kind == c_empty_)
			break;
		if (inf.kind != c_empty_) {
			VERIFY(tmp.ty);VERIFY(inf.ty);
			if (tmp.ty != csT_typebool() || inf.ty != csT_typebool())
				c_emsg_(pos->pos,"and expression type must be bool");
			c_emitcode_(csC_and,c_empty_addr_);
		} 
		inf = tmp;
	}
	return inf;
}

static c_info_ c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo,csG_bool emit)
{
	INITINF(inf);
	VERIFY(foo);
	csA_relexpr rel = csA_urelexprrel(foo);
	VERIFY(rel);
	inf = c_relexpr_(vtab,ttab,rel,emit);
	if (foo->flags) {
		VERIFY (inf.ty == csT_typebool());
		c_emitcode_(csC_not,c_empty_addr_);	
	}
	return inf;
}

static c_info_ c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo,csG_bool emit)
{
	INITINF(inf1);
	VERIFY(foo);
	csA_sumexprlist list1 = csA_relexprsum1(foo);
	VERIFY(list1);
	inf1 = c_sumexprlist_(vtab,ttab,list1,emit);
	csA_op op = csA_relexprop(foo);
	if (op) {
		csA_sumexprlist list2 = csA_relexprsum2(foo);
		c_info_ inf2 = c_sumexprlist_(vtab,ttab,list2,emit);
		if (inf1.ty != inf2.ty) {
			c_emsg_(foo->pos,"invalid operands to binary sumexpr");
			inf1.kind = c_empty_;
			return inf1;
		} else if (inf1.ty != csT_typeint()) {
			c_emsg_(foo->pos,"invalid operands to binary sumexpr");
			inf1.kind = c_empty_;
			return inf1;
		}
		if (emit) {
			switch (op) {
			case csA_eq:
				c_emitcode_(csC_eq,c_empty_addr_);break;
			case csA_neq:
				c_emitcode_(csC_neq,c_empty_addr_);break;
			case csA_lt:
				c_emitcode_(csC_lt,c_empty_addr_);break;
			case csA_lq:
				c_emitcode_(csC_lq,c_empty_addr_);break;
			case csA_gt:
				c_emitcode_(csC_gt,c_empty_addr_);break;
			case csA_gq:
				c_emitcode_(csC_gq,c_empty_addr_);break;
			default:
				VERIFY(0);
			}
			inf1.kind = c_var_;
		}
		inf1.ty = csT_typebool();
		VERIFY (list2);
	}
	return inf1;
}

static c_info_ c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo,csG_bool emit)
{
	INITINF(inf);
	csA_sumexpr pos;
	VERIFY(foo);
	csA_op op  = 0;
	list_for_each_entry(pos, foo, next) {
		c_info_ tmp = c_termlist_(vtab,ttab,csA_sumexprterm(pos),emit);
		if (op) {
			VERIFY(tmp.ty);
			if (tmp.ty != inf.ty) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				inf.kind = c_empty_;
				break;
			} else if (tmp.ty != csT_typeint() && tmp.ty != csT_typestring()) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				inf.kind = c_empty_;
				break;
			}
			if (emit) {
				if (op == csA_plus)
					c_emitcode_(csC_add,c_empty_addr_);
				else if (op == csA_minus) {
					c_emitcode_(csC_sub,c_empty_addr_);
				} else VERIFY(0);
				tmp.kind = c_var_;
			}
		}
		op = csA_sumexprop(pos);
		inf = tmp;
	}
	return inf;
}

static c_info_ c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo,csG_bool emit)
{
	INITINF(inf);
	csA_term pos;
	VERIFY(foo);
	csA_op op  = 0;
	list_for_each_entry(pos, foo, next) {
		c_info_ tmp = c_uexpr_(vtab,ttab,csA_termuexpr(pos),emit);
		if (op) {
			VERIFY(tmp.ty);
			if (tmp.ty != inf.ty) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				inf.kind = c_empty_;
				break;
			} else if (tmp.ty != csT_typeint()) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				inf.kind = c_empty_;
				break;
			}
			if (emit) {
				if (op == csA_times)
					c_emitcode_(csC_mul,c_empty_addr_);
				else if (op == csA_divide) {
					c_emitcode_(csC_div,c_empty_addr_);
				} else VERIFY(0);
				tmp.kind = c_var_;
			}
		}
		op = csA_termop(pos);
		inf = tmp;
	}
	return inf;
}
static c_info_ c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo,csG_bool emit)
{
	INITINF(inf);
	VERIFY(foo);
	
	inf = c_factor_(vtab,ttab,csA_uexprfac(foo),emit);
	if (foo->flags) {
		VERIFY(inf.ty == csT_typeint());
		c_emitcode_(csC_minus,c_empty_addr_);
	}
	return inf;
}

static c_info_ c_factor_(csS_table vtab,csS_table ttab,csA_factor foo,csG_bool emit)
{
	INITINF(inf);
	VERIFY(foo);
	switch (foo->kind) {
	case csA_immut:
		inf = c_immutable_(vtab,ttab,csA_factorimmut(foo),emit);
		break;
	case csA_mut:
		inf = c_mutable_(vtab,ttab,csA_factormut(foo),emit);
		break;
	default:
		VERIFY(0);
	}
	return inf;
}

static c_info_ c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo,csG_bool emit)
{
	INITINF(inf);
	VERIFY(foo);
	switch (foo->kind) {
    case csA_num_:
    	inf.kind = c_const_;
    	inf.u.intconst = csA_immutnum(foo);
    	inf.ty = csT_typeint();
    	if (emit)
   			c_emitcode_(csC_load,c_address_int_(inf.u.intconst));
    	break;
    case csA_char_:
    	VERIFY(0);
    	break;
    case csA_str_:
    	inf.kind = c_const_;
    	inf.u.strconst = csA_immutstr(foo);
    	inf.ty = csT_typestring();
   		if (emit)
    		c_emitcode_(csC_load,c_address_string_(inf.u.strconst));
    	break;
    case csA_bool_:
    	inf.kind = c_const_;
    	inf.u.boolconst = csA_immutbool(foo);
    	inf.ty = csT_typebool();
    	if (emit)
    		c_emitcode_(csC_load,c_address_bool_(inf.u.boolconst));
    	break;
    case csA_call_:{
    	csA_arglist arglist = csA_immutcallargs(foo);
    	csS_symbol funname = csA_immutcallid(foo);
    	VERIFY(funname);
    	csE_enventry e = csS_look(vtab,funname);
    	if (!e) {
    		c_emsg_(csA_immutpos(foo),"implicit declaration of function %s",csS_name(funname));
    		return inf;
    	}
    	VERIFY(e->kind == csE_fun);
    	int count = 0;
    	csT_type resty = e->u.fun.res;
    	csT_typelist formals = e->u.fun.formals;
    	t_typelistentry_ pos1 = NULL;
    	csA_expr pos2 = NULL;
    	c_emitcode_(csC_mark,c_empty_addr_);
    	if (arglist) {
    		VERIFY(formals);
	    	list_for_each_entry_2(pos1,pos2, &formals->head,arglist, next,next) {
	    		count++;
	    		csT_type ty = pos1->type;
	    		inf = c_expr_(vtab,ttab,pos2,TRUE);
				VERIFY(inf.ty);
				if (inf.ty != ty) {
					c_emsg_(csA_immutpos(foo),"%dth argument type is wrong at function %s",count,csS_name(funname));
				}
	    	}
	    	if (list_is_last(&pos2->next,arglist) != list_is_last(&pos1->next,&formals->head)) {
    			c_emsg_(csA_immutpos(foo),"wrong number of arguments at function %s",csS_name(funname));
    		}
    	} else {
    		VERIFY(formals);
    		if (formals->count != 0)
    			c_emsg_(csA_immutpos(foo),"wrong number of arguments at function %s",csS_name(funname));
    	}
    	c_emitcode_(csC_cup,c_address_env_(e));
    	break;
    }
    default:
    	VERIFY(0);
	}
	return inf;
}

static c_info_ c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo,csG_bool emit)
{
	INITINF(inf);
	VERIFY(foo);
	csS_symbol id = csA_mutid(foo);
	csE_enventry e = csS_look(vtab, id);
	if (!e) {
		c_emsg_(foo->pos,"unknown variable %s",csS_name(id));
		return inf;
	}
	VERIFY(e->kind == csE_var);
	inf.ty = e->u.var.type;
	VERIFY(inf.ty);
	inf.kind = c_env_;
	inf.u.env = e;
	if (emit)
		c_emitcode_(csC_load,c_address_env_(e));
	return inf;
}


static void c_emsg_(csG_pos pos,char *message,...)
{
	va_list ap;
	csG_error = TRUE;
	VERIFY(message);
	if (filename) fprintf(errors,"%s:",filename);
	fprintf(errors,"%d:%d:",pos.row,pos.col);
	va_start(ap,message);
	vfprintf(errors, message, ap);
	fprintf(errors,"\n");
	va_end(ap);
}

static void c_frag_(csF_access access,csC_fraglist fraglist,c_info_ inf)
{
	VERIFY(access);
	VERIFY(fraglist);
	VERIFY(inf.kind != c_empty_);
}

static csC_quadlist c_quadlist_()
{
	csC_quadlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	c_quad_ = foo;
	return foo;
}

static void c_emitcode_(c_quadkind_ kind,csC_address addr)
{
	if(!c_quad_ || csG_error)
		return ;
	csC_quad foo = csU_malloc(sizeof(*foo));
	foo->arg = addr;
	foo->kind = kind;
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, c_quad_);
}


static csC_address c_address_int_(int intconst)
{
	csC_address addr;
	addr.kind = csC_intconst;
	addr.u.ival = intconst;
	return addr;
}

static csC_address c_address_string_(csG_string strconst)
{
	csC_address addr;
	addr.kind = csC_strconst;
	addr.u.str = strconst;
	return addr;
}

static csC_address c_address_bool_(csG_bool boolconst)
{
	csC_address addr;
	addr.kind = csC_boolconst;
	addr.u.bval = boolconst;
	return addr;
}

static csC_address c_address_env_(csE_enventry eval)
{
	csC_address addr;
	addr.kind = csC_env;
	addr.u.eval = eval;
	VERIFY(eval);
	return addr;
}

static csC_address c_address_lable_(csT_label lab)
{
	VERIFY(lab);
	csC_address addr;
	addr.kind = caC_lable;
	addr.u.lab = lab;
	return addr;
}

static csC_frag c_procfrag_(csC_quadlist body,csF_frame frame,csC_fraglist fraglist)
{
	VERIFY(body);VERIFY(frame);VERIFY(fraglist);
	csC_frag foo = csU_malloc(sizeof(*foo));
	foo->kind = csC_procfrag;
	foo->u.proc.body = body;
	foo->u.proc.frame = frame;
	list_add_tail(&foo->next, fraglist);
	return foo;
}


static c_info_ c_infoconst_(csT_type ty)
{
	INITINF(inf);
	inf.kind = c_const_;
	inf.ty = ty;
	return inf;
}
