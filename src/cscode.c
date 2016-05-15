#include "cscode.h"
#include "cssymbol.h"
#include "cslist.h"
#include "csutil.h"
#include <string.h>
#include <stdarg.h>

csC_fraglist fraglist = LIST_HEAD_INIT(fraglist);
static csC_quadlist quadlist;
static csF_frame curframe;
static csT_type c_returnty_;


static csC_info c_info_();
static csC_quadlist c_quadlist_();
static csC_frag c_frag_(csF_access access,csC_info inf);
static csC_frag c_procfrag_(csC_quadlist body,csF_frame frame);
static csC_address c_address_();
static csC_address c_addressint_(int intconst);
static csC_address c_addressstr_(csG_string strconst);
static csC_address c_addressbool_(csG_bool boolconst);
static csC_address c_addressenv_(csE_enventry eval);
static csC_address c_addresstemp_(csT_temp tmp);
static csC_address c_addresslable_(csT_label lab);
static csC_quad c_quad_(csC_address arg1,csC_address arg2,csC_address res,c_opkind_ op);
static csC_info c_opdispatch_(csA_op op,csC_info inf,csC_info tmp);
static void c_addrdispatch_(csA_op op,csC_address arg1,csC_address arg2,csC_address res);
static void c_emsg_(csG_pos pos,char *message,...);


static csC_info c_dec_(csS_table val,csS_table type,csA_dec list);
static csC_info c_locdeclist(csS_table vtab,csS_table ttab,csA_locdeclist list);
static csC_info c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo);
static csC_info c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo);
static csC_info c_andexpr_(csS_table vtab,csS_table ttab,csA_andexpr foo);
static csC_info c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo);
static csC_info c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo);
static csC_info c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo);
static csC_info c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo);
static csC_info c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo);
static csC_info c_factor_(csS_table vtab,csS_table ttab,csA_factor foo);
static csC_info c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo);
static csC_info c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo);
static csC_info c_stmtlist_(csS_table vtab,csS_table ttab,csA_stmtlist foo,csT_label lable);
static csC_info c_exprlist_(csS_table vtab,csS_table ttab,csA_exprlist foo);
static csC_info c_expr_(csS_table vtab,csS_table ttab,csA_expr foo);
static csC_info c_stmt_(csS_table vtab,csS_table ttab,csA_stmt foo,csT_label lable);
static csC_info c_arglist_(csS_table vtab,csS_table ttab,csA_arglist foo);


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
	list_add_tail(&foo->next, &fraglist);
	return foo;
}

static csC_frag c_procfrag_(csC_quadlist body,csF_frame frame)
{
	VERIFY(body);VERIFY(frame);
	csC_frag foo = csU_malloc(sizeof(*foo));
	foo->kind = csC_procfrag;
	foo->u.proc.body = body;
	foo->u.proc.frame = frame;
	list_add_tail(&foo->next, &fraglist);
	return foo;
}

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

static csC_address c_infotoaddr(csC_info inf)
{
	csC_address addr;
	switch (inf.kind) {
		case c_addr_:
			addr = inf.u.addr;
			break;
		case c_intconst_:
			addr = c_addressint_(inf.u.intconst);
			break;
		case c_strconst_:
			addr = c_addressstr_(inf.u.strconst);
			break;
		case c_boolconst_:
			addr = c_addressbool_(inf.u.boolconst);
			break;
		case c_empty_:
			return c_address_();
		default:
			VERIFY(0);
	}
	return addr;
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
	case csA_vardec: {
		quadlist = NULL;
		csS_symbol tyname = csA_decvartype(foo);
		csS_symbol name = csA_decvarname(foo);
		csT_type ty = csS_look(ttab, tyname);
		if(!ty) {
			c_emsg_(foo->pos,"unknown type %s",csS_name(tyname));
			break;
		} else if (ty == csT_typevoid()) {
			c_emsg_(foo->pos,"variable %s declared void",csS_name(name));
			break;
		}
		csE_enventry e = csS_look(vtab, name);
		if(e) c_emsg_(foo->pos,"redefinition of variable %s",csS_name(name));
		csF_access access = NULL;
		if (ty) access = csF_allocglobal(ty);
		e = csE_varentry(ty,access,name);
		csS_insert(vtab, name, e);
		csA_simplelist list = csA_decvarlist(foo);
		if (list) {
			csC_info info = c_simplelist_(vtab,ttab,list);
			VERIFY(info.ty);
			if (ty != info.ty) 
				c_emsg_(foo->pos,"incompatible types when assigning");
			c_frag_(access, info);
		} else {
			c_frag_(access, c_infoconst_(ty));
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
		curframe = frame;
		e = csE_funentry(list,ty,name,frame);
		csS_insert(vtab, name, e);
		{
			csS_beginscope(vtab);
			csA_param pos = NULL;
			int regnum = 0;
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
					regnum++;
				}
			}
			c_quadlist_();
			c_quad_(c_address_(),c_address_(),c_addressenv_(e),csC_func);
			csA_locdeclist list = csA_decfunloclist(foo);
			int ttt = 0;
			if (list) {
				csC_info i = c_locdeclist(vtab,ttab,list);
				ttt = i.u.intconst;
			}
			frame->framesize = ttt + regnum;
			csA_stmtlist stmt = csA_decfunstmtlist(foo);
			if (stmt)
				c_stmtlist_(vtab,ttab,stmt,NULL);
			csS_endscope(vtab);
		}
		c_procfrag_(quadlist,frame);
		break;
	}
	default:
		VERIFY(0);
	}
	return inf;
}


static csC_info c_locdeclist(csS_table vtab,csS_table ttab,csA_locdeclist list)
{
	csC_info inf = c_info_();
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
		csF_access access = csF_alloclocal(curframe);
		csA_simplelist list = csA_locdecsimlist(pos);
		if (list) {
			csC_info tmp = c_simplelist_(vtab,ttab,list);
			csE_enventry env = csE_varentry(ty,access,name);
			VERIFY(tmp.ty);
			if (ty != tmp.ty) 
				c_emsg_(pos->pos,"incompatible types when assigning");
			csC_address arg1 = c_infotoaddr(tmp);
			csC_address arg2 = c_address_();
			csC_address res = c_addressenv_(env);
			c_quad_(arg1,arg2,res,csC_assign);
		}
		csS_insert(vtab, name, csE_varentry(ty,access,name));
		varcount++;
	}
	inf.kind = c_intconst_;
	inf.u.intconst = varcount;
	return inf;
}

static csC_info c_expr_(csS_table vtab,csS_table ttab,csA_expr foo)
{
	VERIFY(foo);
	csC_info inf = c_info_();
	switch (foo->kind) {
	case csA_sim_:
		VERIFY(csA_exprsimplelist(foo));
		inf = c_simplelist_(vtab,ttab,csA_exprsimplelist(foo));
		break;
	case csA_asgn_: {
		VERIFY(csA_exprmut(foo));
		inf = c_mutable_(vtab,ttab,csA_exprmut(foo));
		csT_type mutty = inf.ty;
		//VERIFY(mutty);
		
		csC_address res = c_infotoaddr(inf);
		csC_address arg2 = c_address_();
		VERIFY(csA_exprexpr(foo));
		inf = c_expr_(vtab,ttab,csA_exprexpr(foo));
		csT_type exprty = inf.ty;
		VERIFY(exprty);
		if (exprty != mutty) 
			c_emsg_(foo->pos,"incompatible types when assigning");
		csC_address arg1 = c_infotoaddr(inf);
		c_quad_(arg1,arg2,res,csC_assign);
		break;
	}
	default:
		VERIFY(0);
	}
	return inf;
}

static csC_info c_exprlist_(csS_table vtab,csS_table ttab,csA_exprlist foo)
{
	csC_info inf = c_info_();
	csA_expr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		inf = c_expr_(vtab,ttab,pos);
	}
	return inf;
}

static csC_info c_stmt_(csS_table vtab,csS_table ttab,csA_stmt pos,csT_label lable)
{
	csC_info inf = c_info_();
	VERIFY(pos);
	switch (pos->kind) {
		case csA_exprstmt:
			if (pos->u.exprList)
				inf = c_exprlist_(vtab,ttab,pos->u.exprList);
			break;
		case csA_ifstmt:{//struct { csA_exprlist list;csA_stmt ifs,elses;} ifstmt;
			VERIFY(pos->u.ifstmt.list);
			inf = c_exprlist_(vtab,ttab,pos->u.ifstmt.list);
			csT_label L1 = csT_newlabel();
			csC_address res = c_addresslable_(L1);
			csC_address arg1 = c_infotoaddr(inf);
			c_quad_(arg1,c_address_(),res,csC_iffalse);
			VERIFY(pos->u.ifstmt.ifs);
			inf = c_stmt_(vtab,ttab,pos->u.ifstmt.ifs,lable);
			csA_stmt elses = pos->u.ifstmt.elses;
			csT_label L2 = NULL;
			if (elses) {
				L2 = csT_newlabel();
				c_quad_(c_address_(),c_address_(),c_addresslable_(L2),csC_goto);
			}
			c_quad_(c_address_(),c_address_(),res,csC_lable);
			if (elses) {
				inf = c_stmt_(vtab,ttab,elses,lable);
				c_quad_(c_address_(),c_address_(),c_addresslable_(L2),csC_lable);
			}
			break;
		}//struct { csA_locdeclist varlist;csA_stmtlist stmtlist ;} comstmt;
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
			csC_address res = c_addresslable_(in);
			csC_address arg1 = c_address_();
			c_quad_(arg1,arg1,res,csC_lable);
			inf = c_exprlist_(vtab,ttab,list);
			VERIFY(inf.ty);
			if (inf.ty != csT_typebool()) 
				c_emsg_(pos->pos,"expression type in while statement must be bool");
			arg1 = c_infotoaddr(inf);
			c_quad_(arg1,c_address_(),c_addresslable_(out),csC_iffalse);
			inf = c_stmt_(vtab,ttab,stmt,out);
			c_quad_(c_address_(),c_address_(),c_addresslable_(in),csC_goto);
			c_quad_(c_address_(),c_address_(),c_addresslable_(out),csC_lable);
			break;
		}
		case csA_forstmt: {
			csA_exprlist list1 = pos->u.forstmt.list1;
			csA_exprlist list2 = pos->u.forstmt.list2;
			csA_exprlist list3 = pos->u.forstmt.list3;
			csA_stmt stmt = pos->u.forstmt.stmt;
			if (list1)
				inf = c_exprlist_(vtab,ttab,list1);
			csT_label loopbody = csT_newlabel();
			csT_label out = csT_newlabel();
			
			csC_address arg1 = c_address_();
			if (list2) {
				inf = c_exprlist_(vtab,ttab,list2);
				VERIFY(inf.ty);
				if (inf.ty != csT_typebool()) 
					c_emsg_(pos->pos,"2th expression type in for statement must be bool");
				arg1 = c_infotoaddr(inf);
				c_quad_(arg1,c_address_(),c_addresslable_(out),csC_iffalse);
			}
			c_quad_(c_address_(),c_address_(),c_addresslable_(loopbody),csC_lable);
			VERIFY(stmt);
			inf = c_stmt_(vtab,ttab,stmt,out);
			if (list3)
				inf = c_exprlist_(vtab,ttab,list3);
			if (list2) {
				inf = c_exprlist_(vtab,ttab,list2);
				arg1 = c_infotoaddr(inf);
				c_quad_(arg1,c_address_(),c_addresslable_(loopbody),csC_if);
				c_quad_(c_address_(),c_address_(),c_addresslable_(out),csC_lable);
			} else {
				c_quad_(arg1,c_address_(),c_addresslable_(loopbody),csC_goto);
			}
			break;
		}
		case csA_returnstmt: {
			VERIFY(c_returnty_);
			csA_exprlist list = pos->u.retstmt.list;
			csC_address arg1 = c_address_();
			if (list && c_returnty_ == csT_typevoid()) {
				c_emsg_(pos->pos,"return with a value, in function returning void");
			} else if (!list && c_returnty_ != csT_typevoid()) {
				c_emsg_(pos->pos,"with no value, in function returning non-void");
			} else if (list) {
				inf = c_exprlist_(vtab,ttab,list);
				arg1 = c_infotoaddr(inf);
				if (!csG_error)
					VERIFY(inf.ty);
				if (inf.ty != c_returnty_)
					c_emsg_(pos->pos,"return type is wrong");
			}
			c_quad_(c_address_(),c_address_(),arg1,csC_return);
			break;
		}
		case csA_breakstmt: {
			if (!lable)
				c_emsg_(pos->pos,"break statement not whthin for or while statement");
			else
				c_quad_(c_address_(),c_address_(),c_addresslable_(lable),csC_goto);
			break;
		}
		default:
			VERIFY(0);
	}
	return inf;
}

static csC_info c_stmtlist_(csS_table vtab,csS_table ttab,csA_stmtlist foo,csT_label lable)
{
	VERIFY(foo);
	csC_info inf = c_info_();
	csA_stmt pos;
	list_for_each_entry(pos, foo, next) {
		inf = c_stmt_(vtab,ttab,pos,lable);
	}
	return inf;
}

static csC_info c_simplelist_(csS_table vtab,csS_table ttab,csA_simplelist foo)
{
	csC_info inf = c_info_();
	csA_simpleexpr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		csC_info tmp = c_andlist_(vtab,ttab,csA_simpleexprand(pos));
		if (inf.kind != c_empty_) {
			VERIFY(tmp.ty);
			VERIFY(tmp.ty->kind == csT_bool);
			if (tmp.kind == c_boolconst_ && inf.kind == c_boolconst_)
				tmp.u.boolconst = inf.u.boolconst || tmp.u.boolconst;
			else {
				csC_address arg1 = c_infotoaddr(inf);
				csC_address arg2 = c_infotoaddr(tmp);
				csC_address res = c_addresstemp_(csT_newtemp());
				c_quad_(arg1,arg2,res,csC_or);
				tmp.u.addr = res;
				tmp.kind = c_addr_;
			}
		}
		inf = tmp;
	}
	return inf;
}

static csC_info c_andlist_(csS_table vtab,csS_table ttab,csA_andlist foo)
{
	csC_info inf = c_info_();
	csA_andexpr pos;
	VERIFY(foo);
	list_for_each_entry(pos, foo, next) {
		csC_info tmp = c_urelexpr_(vtab,ttab,csA_andexprurel(pos));
		if (inf.kind != c_empty_) {
			VERIFY(tmp.ty);VERIFY(inf.ty);
			if (tmp.ty != csT_typebool() || inf.ty != csT_typebool())
				c_emsg_(pos->pos,"and expression type must be bool");
			if (tmp.kind == c_boolconst_ && inf.kind == c_boolconst_)
				tmp.u.boolconst = inf.u.boolconst && tmp.u.boolconst;
			else {
				csC_address arg1 = c_infotoaddr(inf);
				csC_address arg2 = c_infotoaddr(tmp);
				csC_address res = c_addresstemp_(csT_newtemp());
				c_quad_(arg1,arg2,res,csC_and);
				tmp.u.addr = res;
				tmp.kind = c_addr_;
			}
		} 
		inf = tmp;
	}
	return inf;
}

static csC_info c_urelexpr_(csS_table vtab,csS_table ttab,csA_urelexpr foo)
{
	csC_info inf = c_info_();
	VERIFY(foo);
	csA_relexpr rel = csA_urelexprrel(foo);
	VERIFY(rel);
	inf = c_relexpr_(vtab,ttab,rel);
	if (foo->flags) {
		if (inf.kind == c_boolconst_) {
			inf.u.boolconst = !inf.u.boolconst;
			return inf;
		}
		csC_address arg1,arg2,res;
		res = c_addresstemp_(csT_newtemp());
		arg1 = c_infotoaddr(inf);
		c_quad_(arg1,c_address_(),res,csC_not);
		inf.kind = c_addr_;
		inf.u.addr = res;
	}
	return inf;
}

static csC_info c_relexpr_(csS_table vtab,csS_table ttab,csA_relexpr foo)
{
	csC_info inf1 = c_info_();
	VERIFY(foo);
	csA_sumexprlist list1 = csA_relexprsum1(foo);
	VERIFY(list1);
	inf1 = c_sumexprlist_(vtab,ttab,list1);
	csA_op op = csA_relexprop(foo);
	if (op) {
		csA_sumexprlist list2 = csA_relexprsum2(foo);
		csC_info inf2 = c_sumexprlist_(vtab,ttab,list2);
		if (inf1.kind == c_intconst_ && inf2.kind == c_intconst_) {
			return c_opdispatch_(op,inf1,inf2);
		} else if (inf1.kind == c_strconst_ && inf2.kind == c_strconst_) {
			VERIFY(0);
		} 
		VERIFY (list2);
		csC_address arg1,arg2,res;
		res = c_addresstemp_(csT_newtemp());
		arg1 = c_infotoaddr(inf1);
		arg2 = c_infotoaddr(inf2);
		switch (op) {
		case csA_eq:
			c_quad_(arg1,arg2,res,csC_eq);
			break;
		case csA_neq:
			c_quad_(arg1,arg2,res,csC_neq);
			break;
		case csA_lt:
			c_quad_(arg1,arg2,res,csC_lt);
			break;
		case csA_lq:
			c_quad_(arg1,arg2,res,csC_lq);
			break;
		case csA_gt:
			c_quad_(arg1,arg2,res,csC_gt);
			break;
		case csA_gq:
			c_quad_(arg1,arg2,res,csC_gq);
			break;
		default:
			VERIFY(0);
		}
		inf1.kind = c_addr_;
		inf1.u.addr = res;
		inf1.ty = csT_typebool();
	}
	return inf1;
}

static csC_info c_sumexprlist_(csS_table vtab,csS_table ttab,csA_sumexprlist foo)
{
	csC_info inf = c_info_();
	csA_sumexpr pos;
	VERIFY(foo);
	csA_op op  = 0;
	list_for_each_entry(pos, foo, next) {
		csC_info tmp = c_termlist_(vtab,ttab,csA_sumexprterm(pos));
		if (op) {
			VERIFY(tmp.ty);
			if (tmp.ty != inf.ty) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				break;
			} else if (tmp.ty != csT_typeint() && tmp.ty != csT_typestring()) {
				c_emsg_(pos->pos,"invalid operands to binary sumexpr");
				break;
			}

			if (tmp.kind == c_intconst_ && inf.kind == c_intconst_) {
				tmp = c_opdispatch_(op,inf,tmp);
			} else if (tmp.kind == c_strconst_ && inf.kind == c_strconst_) {
				VERIFY(0);
			} else {
				csC_address arg1 = c_infotoaddr(inf);
				csC_address arg2 = c_infotoaddr(tmp);
				csC_address res = c_addresstemp_(csT_newtemp());
				c_addrdispatch_(op,arg1,arg2,res);
				tmp.u.addr = res;
				tmp.kind = c_addr_;
				
			}

		}
		op = csA_sumexprop(pos);
		inf = tmp;
	}
	return inf;
}

static csC_info c_termlist_(csS_table vtab,csS_table ttab,csA_termlist foo)
{
	csC_info inf;
	csA_term pos;
	VERIFY(foo);
	csA_op op  = 0;
	list_for_each_entry(pos, foo, next) {
		csC_info tmp = c_uexpr_(vtab,ttab,csA_termuexpr(pos));
		if (op) {
			if (tmp.kind == c_intconst_ && inf.kind == c_intconst_) {
				tmp = c_opdispatch_(op,inf,tmp);
			} else if (tmp.kind == c_strconst_ && inf.kind == c_strconst_) {
				VERIFY(0);
			}  else {
				csC_address arg1 = c_infotoaddr(inf);
				csC_address arg2 = c_infotoaddr(tmp);
				csC_address res = c_addresstemp_(csT_newtemp());
				c_addrdispatch_(op,arg1,arg2,res);
				tmp.u.addr = res;
				tmp.kind = c_addr_;
			}
		}
		op = csA_termop(pos);
		inf = tmp;
	}
	return inf;
}
static csC_info c_uexpr_(csS_table vtab,csS_table ttab,csA_uexpr foo)
{
	csC_info inf;
	VERIFY(foo);
	if (foo->flags)
		VERIFY(0);
	inf = c_factor_(vtab,ttab,csA_uexprfac(foo));
	return inf;
}

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

static csC_info c_immutable_(csS_table vtab,csS_table ttab,csA_immutable foo)
{
	csC_info inf = c_info_();
	VERIFY(foo);
	switch (foo->kind) {
    case csA_num_:
    	inf.kind = c_intconst_;
    	inf.u.intconst = csA_immutnum(foo);
    	inf.ty = csT_typeint();
    	break;
    case csA_char_:
    	VERIFY(0);
    	break;
    case csA_str_:
    	inf.kind = c_strconst_;
    	inf.u.strconst = csA_immutstr(foo);
    	inf.ty = csT_typestring();
    	break;
    case csA_bool_:
    	inf.kind = c_boolconst_;
    	inf.u.boolconst = csA_immutbool(foo);
    	inf.ty = csT_typebool();
    	break;
    case csA_call_:{
    	csA_arglist arglist = csA_immutcallargs(foo);
    	csC_address res = c_addresstemp_(csT_newtemp());
    	csC_address arg1,arg2;
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
    	if (e->u.fun.bulitin) {
    		csA_expr pos = NULL;
    		if (!e->u.fun.formals) {
	    		list_for_each_entry(pos,arglist,next) {
		    		inf = c_expr_(vtab,ttab,pos);
					csC_address res = c_infotoaddr(inf);
					c_quad_(c_address_(),c_address_(),res,csC_param);
					VERIFY(inf.ty);
					count++;
	    		}
	    	} else {
	    		VERIFY(0);
	    	}
    	} else {
	    	csT_typelist formals = e->u.fun.formals;
	    	t_typelistentry_ pos1 = NULL;
	    	csA_expr pos2 = NULL;
	    	if (arglist) {
	    		VERIFY(formals);
		    	list_for_each_entry_2(pos1,pos2, &formals->head,arglist, next,next) {
		    		count++;
		    		csT_type ty = pos1->type;
		    		inf = c_expr_(vtab,ttab,pos2);
					csC_address res = c_infotoaddr(inf);
					c_quad_(c_address_(),c_address_(),res,csC_param);
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
    	}
    	arg1 = c_addressenv_(e);
    	arg2 = c_addressint_(count);
    	c_quad_(arg1,arg2,res,csC_call);
    	inf.kind = c_addr_;
    	inf.u.addr = res;
    	inf.ty = resty;
    	break;
    }
    default:
    	VERIFY(0);
	}
	return inf;
}

static csC_info c_arglist_(csS_table vtab,csS_table ttab,csA_arglist foo)
{
	csC_info inf = c_info_();
	csA_expr pos;
	inf.kind = c_intconst_;
	inf.u.intconst = 0;
	if (!foo) return inf;
	int i = 0;
	list_for_each_entry(pos, foo, next) {
		inf = c_expr_(vtab,ttab,pos);
		csC_address res = c_infotoaddr(inf);
		c_quad_(c_address_(),c_address_(),res,csC_param);
		i++;
	}
	inf.kind = c_intconst_;
	inf.u.intconst = i;
	return inf;
}

static csC_info c_mutable_(csS_table vtab,csS_table ttab,csA_mutable foo)
{
	csC_info inf;
	inf.kind = c_addr_;
	inf.u.addr = c_address_();
	VERIFY(foo);
	csS_symbol id = csA_mutid(foo);
	csE_enventry e = csS_look(vtab, id);
	if (!e) {
		c_emsg_(foo->pos,"unknown variable %s",csS_name(id));
		return inf;
	}
	VERIFY(e->kind == csE_var);
	inf.ty = e->u.var.type;
	if (!csG_error)
		VERIFY(inf.ty);
	
	inf.u.addr = c_addressenv_(e);
	return inf;
}

static csC_address c_address_()
{
	csC_address addr;
	memset(&addr,0,sizeof(addr));
	addr.kind = csC_empty;
	return addr;
}

static csC_address c_addressint_(int intconst)
{
	csC_address addr;
	addr.kind = csC_intconst;
	addr.u.ival = intconst;
	return addr;
}

static csC_address c_addressstr_(csG_string strconst)
{
	csC_address addr;
	addr.kind = csC_strconst;
	addr.u.str = strconst;
	return addr;
}

static csC_address c_addressbool_(csG_bool boolconst)
{
	csC_address addr;
	addr.kind = csC_boolconst;
	addr.u.bval = boolconst;
	return addr;
}

static csC_address c_addressenv_(csE_enventry eval)
{
	csC_address addr;
	addr.kind = csC_env;
	addr.u.eval = eval;
	//VERIFY(eval);
	return addr;
}

static csC_address c_addresstemp_(csT_temp tmp)
{
	csC_address addr;
	addr.kind = csC_temp;
	addr.u.tmp = tmp;
	return addr;
}

static csC_address c_addresslable_(csT_label lab)
{
	VERIFY(lab);
	csC_address addr;
	addr.kind = caC_lable;
	addr.u.lab = lab;
	return addr;
}

static csC_quad c_quad_(csC_address arg1,csC_address arg2,csC_address res,c_opkind_ op)
{
	if(!quadlist || csG_error)
		return NULL;

	csC_quad foo = csU_malloc(sizeof(*foo));
	foo->arg1 = arg1;
	foo->arg2 = arg2;
	foo->res = res;
	foo->kind = op;
	INIT_LIST_HEAD(&foo->next);
	
		list_add_tail(&foo->next, quadlist);
	return foo;
}

static csC_quadlist c_quadlist_()
{
	csC_quadlist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(foo);
	quadlist = foo;
	return foo;
}

static csC_info c_opdispatch_(csA_op op,csC_info inf,csC_info tmp)
{
	switch (op) {
	case csA_plus:
		tmp.u.intconst = inf.u.intconst + tmp.u.intconst;
		break;
	case csA_minus:
		tmp.u.intconst = inf.u.intconst - tmp.u.intconst;
		break;
	case csA_times:
		tmp.u.intconst = inf.u.intconst * tmp.u.intconst;
		break;
	case csA_divide:
		tmp.u.intconst = inf.u.intconst / tmp.u.intconst;
		break;
	case csA_eq:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst == tmp.u.intconst;
		break;
	case csA_neq:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst != tmp.u.intconst;
		break;
	case csA_lt:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst < tmp.u.intconst;
		break;
	case csA_lq:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst <= tmp.u.intconst;
		break;
	case csA_gt:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst > tmp.u.intconst;
		break;
	case csA_gq:
		tmp.kind = c_boolconst_;
		tmp.u.boolconst = inf.u.intconst >= tmp.u.intconst;
		break;
	default:
		VERIFY(0);
	}
	//tmp.ty = csT_typebool();
	return tmp;
}

static void c_addrdispatch_(csA_op op,csC_address arg1,csC_address arg2,csC_address res)
{
	switch (op) {
	case csA_plus:
		c_quad_(arg1,arg2,res,csC_add);
		break;
	case csA_minus:
		c_quad_(arg1,arg2,res,csC_sub);
		break;
	case csA_times:
		c_quad_(arg1,arg2,res,csC_multiply);
		break;
	case csA_divide:
		c_quad_(arg1,arg2,res,csC_divide);
		break;
	default:
		VERIFY(0);
	}
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