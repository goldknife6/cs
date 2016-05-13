#include "csopcodes.h"
#include "cscode.h"
#include "cslist.h"
#include "csparser.h"
#include "csenv.h"
#include "cshash.h"
#include "csutil.h"



static void b_genproc_(csC_frag frag);
static csG_byte o_getreg_(csH_table regtab,csC_address addr);
static csG_2byte o_constregin_int_(long num);
static csG_2byte o_constregin_str_(csG_string str);
static csG_2byte o_constregin_bool_(csG_bool boo);


void csC_printcode();
void csB_genbytecode()
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	c_declist_(val,type,list);

	if (csG_error || list_empty(&fraglist) )
		return;
	csC_printcode();
	printf("\n");
	csC_frag pos = NULL;
	list_for_each_entry(pos, &fraglist, next) {
		switch (pos->kind) {
		case csC_procfrag:
			b_genproc_(pos);
			break;
		case csC_strfrag:
		case csC_intfrag:
		case csC_boolfrag:
			break;
		default:
			VERIFY(0);
		}
	}
}
static int o_startreg_;
static long offset = 1;
typedef struct o_codelist_ {
	csO_code code;
	csL_list next;
} *o_codelist_;

typedef struct o_lablelist_ {
	long offset;
	csT_label lab;
	o_codelist_ code;
	csL_list next;
} *o_lablelist_;

static o_codelist_ b_addcodelist(csO_code code,csL_list *head)
{
	o_codelist_ foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, head);
	foo->code = code;

	return foo;
}
static void b_addlablelist(o_codelist_ code,csT_label lab,long offset,csL_list *head)
{
	o_lablelist_ foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	foo->lab = lab;
	foo->code = code;
	foo->offset = offset;

	list_add_tail(&foo->next, head);
}
csL_list head = LIST_HEAD_INIT(head);
static void b_genproc_(csC_frag frag)
{
	csC_quadlist body = frag->u.proc.body;
	csF_frame frame = frag->u.proc.frame;
	VERIFY(body);VERIFY(frame);
	csC_quad pos = NULL;
	csH_table labtab = csH_tabempty(NULL,NULL,NULL);
	csH_table regtab = csH_tabempty(NULL,NULL,NULL);
	
	csL_list labhead = LIST_HEAD_INIT(labhead);
	csC_address arg1,arg2,res;
	list_for_each_entry(pos, body, next) {
		switch (pos->kind) {
		case csC_func:{
			VERIFY(pos->res.kind == csC_env);
			csE_enventry e = pos->res.u.eval;
			VERIFY(e);
			csF_frame f = e->u.fun.frame;
			VERIFY(f);
			o_startreg_ = f->framesize + 1;
			continue;
		}
		case csC_lable: {
			res = pos->res;
			VERIFY(res.kind == caC_lable);
			csT_label lab = res.u.lab;
			VERIFY(lab);
			VERIFY(!csH_tablook(labtab, lab));
			csH_tabinsert(labtab, lab, (void*)offset);
			//printf("%s %d\n",csS_name(lab),offset );
			continue;
		}
		case csC_assign: {
			csG_byte resloc = o_getreg_(regtab,pos->res);
			csG_byte arg1loc =  o_getreg_(regtab,pos->arg1);
			csO_code code = csO_iABC(OP_MOVE,resloc,arg1loc,0);
			b_addcodelist(code,&head);
			break;
		}
		case csC_goto: {
			csT_label lab = pos->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csO_code code = csO_iAsBx(OP_JMP,0,l-offset-1);
			o_codelist_ c = b_addcodelist(code,&head);
			if (!l) {
				b_addlablelist(c,lab,offset,&labhead);
			}
			break;
		}
		case csC_not:
			break;
		case csC_iffalse:{
			csT_label lab = pos->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csG_byte arg1loc =  o_getreg_(regtab,pos->arg1);
			csO_code code = csO_iAsBx(OP_IFFALSE,arg1loc,l-offset-1);
			o_codelist_ c = b_addcodelist(code,&head);
			if (!l) {
				b_addlablelist(c,lab,offset,&labhead);
			}
			break;
		}
		case csC_if:{
			csT_label lab = pos->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csG_byte arg1loc =  o_getreg_(regtab,pos->arg1);
			csO_code code = csO_iAsBx(OP_IF,arg1loc,l-offset-1);
			o_codelist_ c = b_addcodelist(code,&head);
			if (!l) {
				b_addlablelist(c,lab,offset,&labhead);
			}
			break;
		}
		case csC_add: {
			csG_byte arg1loc = o_getreg_(regtab,pos->arg1);
			csG_byte arg2loc =  o_getreg_(regtab,pos->arg2);
			csG_byte resloc = o_getreg_(regtab,pos->res);
			csO_code code = csO_iABC(OP_ADD,resloc,arg1loc,arg2loc);
			b_addcodelist(code,&head);
			break;
		}
		case csC_call:
		case csC_param:
		case csC_minus:
		case csC_multiply:
		case csC_divide:
		case csC_sub:
		case csC_or:
		case csC_and:
			break;
		case csC_return:{
			csO_code code = csO_iABC(OP_RETURN,0,0,0);
			b_addcodelist(code,&head);
			break;
		}
		case csC_eq:
		case csC_neq:
		case csC_lt:
		case csC_lq:
		case csC_gt:
		case csC_gq:
			break;
		default:
			VERIFY(0);
		}
		offset++;
	}

	o_lablelist_ ll = NULL;
	list_for_each_entry(ll, &labhead, next) {
		csO_code code = ll->code->code;
		csT_label lab = ll->lab;
		long l = (long)csH_tablook(labtab, lab);
		VERIFY(l);
		switch (csO_iop(code)) {
		case OP_JMP:
			ll->code->code = csO_iAsBx(OP_JMP,0,l-ll->offset);
			break;
		case OP_IF:
			ll->code->code = csO_iAsBx(OP_IF,csO_iA(code),l-ll->offset-1);
			break;
		case OP_IFFALSE:
			ll->code->code = csO_iAsBx(OP_IFFALSE,csO_iA(code),l-ll->offset-1);
			break;
		default:
			VERIFY(0);
		}
	}
	o_codelist_ cl = NULL;
	list_for_each_entry(cl, &head, next) {
		csO_printcode(cl->code);
	}
}
static csG_byte o_getreg_(csH_table regtab,csC_address addr)
{
	csG_byte foo;
	switch (addr.kind) {
	case csC_temp:{
		int tmp = addr.u.tmp->num;
		int reg = (int)(long)csH_tablook(regtab, (void*)(long)tmp);
		if (!reg) {
			csH_tabinsert(regtab, (void*)(long)tmp, (void*)(long)o_startreg_);
			foo = o_startreg_++;
			return foo;
		}
		foo = reg;
		break;
	}
	case csC_env: {
		csE_enventry e = addr.u.eval;
		VERIFY(e);
		VERIFY(e->kind == csE_var);
		csF_access access = e->u.var.access;
		VERIFY(access);
		if (access->kind == f_reg) {
			VERIFY(access->u.reg);
			foo = access->u.reg->num;
		} else if (access->kind == f_intstatic || 
				access->kind == f_strstatic ||
				access->kind == f_boolstatic) {
			VERIFY(0);
		} else {
			VERIFY(0);
		}
		break;
	}
	case csC_intconst:{
		foo = o_startreg_++;
		csG_2byte bx = o_constregin_int_(addr.u.ival);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,&head);
		offset++;
		break;
	}
	case csC_strconst:{
		foo = o_startreg_++;
		csG_2byte bx = o_constregin_str_(addr.u.str);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,&head);
		offset++;
		break;
	}
	case csC_boolconst:{
		foo = o_startreg_++;
		csG_2byte bx =  o_constregin_bool_(addr.u.bval);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,&head);
		offset++;
		break;
	}
	default:
		VERIFY(0);
	}
	return foo;
}

static csH_table inttab;
static csH_table strtab;
static csH_table booltab;
static long regoffset = 1;
static csG_2byte o_constregin_int_(long num)
{
	if (!inttab) inttab = csH_tabempty(NULL,NULL,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(inttab, (void*)num);
	if (!off) {
		csH_tabinsert(inttab, (void*)num, (void*)regoffset);
		return regoffset++;
	}
	return off;
}

static csG_2byte o_constregin_str_(csG_string str)
{
	if (!strtab) strtab = csH_tabempty(csU_strhash,csU_strequal,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(strtab, str);
	if (!off) {
		csH_tabinsert(strtab, str, (void*)regoffset);
		return regoffset++;
	}
	return off;
}

static csG_2byte o_constregin_bool_(csG_bool boo)
{
	if (!booltab) booltab = csH_tabempty(NULL,NULL,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(booltab, (void*)(long)boo);
	if (!off) {
		csH_tabinsert(booltab, (void*)(long)boo, (void*)regoffset);
		return regoffset++;
	}
	return off;
}
