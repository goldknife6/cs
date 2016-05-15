#include "csopcodes.h"
#include "csparser.h"
#include "csenv.h"
#include "cshash.h"
#include "csutil.h"
#include "csbytec.h"

static csG_2byte b_const_offset_ = 1;



static void b_regin_();
static void b_bytecode_(FILE *outs);
static csL_list b_regin_head_ = LIST_HEAD_INIT(b_regin_head_);

static void b_regin_static_bool_(csG_bool val,int offset);
static void b_regin_static_int_(int val,int offset);
static void b_regin_static_str_(csG_string val,int offset);
static void b_regin_const_bool_(csG_bool val,int offset);
static void b_regin_const_int_(int val,int offset);
static void b_regin_const_str_(csG_string val,int offset);


static void b_regin_proc_(csL_list *head,int size,int offset);
static int b_genproc_(csC_quadlist body,csL_list *head);
static csB_codelist b_addcodelist(csO_code code,csL_list *head);
static void b_addlablelist(csB_codelist code,csT_label lab,int offset,csL_list *head);

static csG_byte o_getreg_(csH_table regtab,csC_address addr,int *offset,int *reg,csL_list *head);
static csG_2byte b_constregin_int_(int num);
static csG_2byte b_constregin_bool_(csG_bool boo);
static csG_2byte b_constregin_str_(csG_string str);
static void b_backpatch_(csH_table labtab,csL_list *head);

extern void csC_printcode();
void csB_genbytecode()
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	c_declist_(val,type,list);

	if (csG_error || list_empty(&fraglist) )
		return;

	//csC_printcode();
	b_regin_();
	b_bytecode_(outs);
}



static void b_regin_()
{
	csC_frag o_pos_ = NULL;
	list_for_each_entry(o_pos_, &fraglist, next) {
		switch (o_pos_->kind) {
		case csC_procfrag: {
			csL_list* prochead = NULL;
			prochead = csU_malloc(sizeof(*prochead));
			INIT_LIST_HEAD(prochead);
			csF_frame frame = o_pos_->u.proc.frame;
			csC_quadlist body = o_pos_->u.proc.body;
			VERIFY(body);
			VERIFY(frame);
			int count = b_genproc_(body,prochead);
			b_regin_proc_(prochead,count*4,frame->offset);
			break;
		}
		case csC_strfrag: {
			csF_access access = o_pos_->access;
			VERIFY(access);
			VERIFY(access->kind == f_static);
			b_regin_static_str_(o_pos_->u.strv,access->u.offset);
			break;
		}
		case csC_intfrag:{
			csF_access access = o_pos_->access;
			VERIFY(access);
			VERIFY(access->kind == f_static);
			b_regin_static_int_(o_pos_->u.intv,access->u.offset);
			break;
		}
		case csC_boolfrag:{
			csF_access access = o_pos_->access;
			VERIFY(access);
			VERIFY(access->kind == f_static);
			b_regin_static_bool_(o_pos_->u.boolv,access->u.offset);
			break;
		}
		default:
			VERIFY(0);
		}
	}

}

static void b_bytecode_(FILE *outs)
{
	VERIFY(outs);
	csB_regin o_pos_ = NULL;
	csF_format format;
	int staticsize = 0;
	int constsize = 0;
	int procsize = 0;
	list_for_each_entry(o_pos_, &b_regin_head_, next) {
		switch (o_pos_->kind) {
		case f_static_:
			staticsize++;
			break;
		case f_prco_:
			procsize++;
			break;
		case f_const_:
			constsize++;
			break;
		default:
			VERIFY(0);
		}
	}
	csF_fmtheader header = csF_header(staticsize,constsize,procsize);
	fwrite(&header, sizeof(header), 1, outs);

	list_for_each_entry(o_pos_, &b_regin_head_, next) {
		switch (o_pos_->kind) {
		case f_static_:{
			format = csF_static(o_pos_->u.val.kind,o_pos_->size,o_pos_->offset);
			fwrite(&format, sizeof(format), 1, outs);
			switch (o_pos_->u.val.kind) {
			case f_int_:
				fwrite(&o_pos_->u.val.u.intv, o_pos_->size, 1, outs);
				break;
			case f_str_:
				fwrite(o_pos_->u.val.u.strv, o_pos_->size, 1, outs);
				break;
			case f_bool_:
				fwrite(&o_pos_->u.val.u.boolv, o_pos_->size, 1, outs);
				break;
			default:
				VERIFY(0);	
			}
			break;
		}
		case f_prco_:{
			format = csF_proc(o_pos_->size,o_pos_->offset);
			fwrite(&format, sizeof(format), 1, outs);
			VERIFY(o_pos_->u.head);
			csB_codelist p_code_;
			csO_code code;
			list_for_each_entry(p_code_, o_pos_->u.head, next) {
				code = p_code_->code;
				csO_printcode(code);
				fwrite(&code.u, sizeof(code), 1, outs);
			}
			break;
		}
		case f_const_:{
			format = csF_const(o_pos_->u.val.kind,o_pos_->size,o_pos_->offset);
			fwrite(&format, sizeof(format), 1, outs);
			switch (o_pos_->u.val.kind) {
			case f_int_:
				fwrite(&o_pos_->u.val.u.intv, o_pos_->size, 1, outs);
				break;
			case f_str_:
				fwrite(o_pos_->u.val.u.strv, o_pos_->size, 1, outs);
				break;
			case f_bool_:
				fwrite(&o_pos_->u.val.u.boolv, o_pos_->size, 1, outs);
				break;
			default:
				VERIFY(0);	
			}
			break;
		}
		default:
			VERIFY(0);
		}
		
	}
}


static int b_genproc_(csC_quadlist body,csL_list *head)
{
	int p_startreg_ = 0;
	csC_quad p_pos_ = NULL;
	int p_count_ = 0;
	VERIFY(body);
	csH_table labtab = csH_tabempty(NULL,NULL,NULL);
	csH_table regtab = csH_tabempty(NULL,NULL,NULL);
	csL_list labhead = LIST_HEAD_INIT(labhead);

	list_for_each_entry(p_pos_, body, next) {
		switch (p_pos_->kind) {
		case csC_func:{
			VERIFY(p_pos_->res.kind == csC_env);
			csE_enventry e = p_pos_->res.u.eval;
			VERIFY(e);
			csF_frame f = e->u.fun.frame;
			VERIFY(f);
			p_startreg_ = f->framesize + 1;
			continue;
		}
		case csC_lable: {
			VERIFY(p_pos_->res.kind == caC_lable);
			csT_label lab = p_pos_->res.u.lab;
			VERIFY(lab);
			VERIFY(!csH_tablook(labtab, lab));
			csH_tabinsert(labtab, lab, (void*)(long)p_count_);
			continue;
		}
		case csC_assign: {
			csG_byte resloc = o_getreg_(regtab,p_pos_->res,&p_count_,&p_startreg_,head);
			csG_byte arg1loc =  o_getreg_(regtab,p_pos_->arg1,&p_count_,&p_startreg_,head);
			csO_code code = csO_iABC(OP_MOVE,resloc,arg1loc,0);
			b_addcodelist(code,head);
			break;
		}
		case csC_goto: {
			csT_label lab = p_pos_->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csO_code code = csO_iAsBx(OP_JMP,0,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_not:
			VERIFY(0);
			break;
		case csC_iffalse:{
			csT_label lab = p_pos_->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csG_byte arg1loc =  o_getreg_(regtab,p_pos_->arg1,&p_count_,&p_startreg_,head);
			csO_code code = csO_iAsBx(OP_IFFALSE,arg1loc,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_if:{
			csT_label lab = p_pos_->res.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			csG_byte arg1loc =  o_getreg_(regtab,p_pos_->arg1,&p_count_,&p_startreg_,head);
			csO_code code = csO_iAsBx(OP_IF,arg1loc,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_add: {
			csG_byte arg1loc = o_getreg_(regtab,p_pos_->arg1,&p_count_,&p_startreg_,head);
			csG_byte arg2loc =  o_getreg_(regtab,p_pos_->arg2,&p_count_,&p_startreg_,head);
			csG_byte resloc = o_getreg_(regtab,p_pos_->res,&p_count_,&p_startreg_,head);
			csO_code code = csO_iABC(OP_ADD,resloc,arg1loc,arg2loc);
			b_addcodelist(code,head);
			break;
		}
		case csC_call:{
			csG_byte arg1loc = o_getreg_(regtab,p_pos_->arg1,&p_count_,&p_startreg_,head);
			VERIFY(p_pos_->arg2.kind == csC_intconst);
			csG_byte arg2loc =  p_pos_->arg2.u.ival;
			csG_byte resloc = o_getreg_(regtab,p_pos_->res,&p_count_,&p_startreg_,head);
			csO_code code = csO_iABC(OP_CALL,resloc,arg1loc,arg2loc);
			b_addcodelist(code,head);
			break;
		}
		case csC_param: {
			csG_byte resloc = o_getreg_(regtab,p_pos_->res,&p_count_,&p_startreg_,head);
			csO_code code = csO_iABC(OP_PARAM,resloc,0,0);
			b_addcodelist(code,head);
			break;
		}
		case csC_minus:
		case csC_multiply:
		case csC_divide:
		case csC_sub:
		case csC_or:
		case csC_and:
			p_count_--;
			break;
		case csC_return:{
			csO_code code = csO_iABC(OP_RETURN,0,0,0);
			b_addcodelist(code,head);
			break;
		}
		case csC_eq:
		case csC_neq:
		case csC_lt:
		case csC_lq:
		case csC_gt:
		case csC_gq:
			VERIFY(0);
			break;
		default:
			VERIFY(0);
		}

		p_count_++;
	}

	b_backpatch_(labtab,&labhead);

	return p_count_;
}

static csG_byte o_getreg_(csH_table regtab,csC_address addr,int *offset,int *statrreg,csL_list *head)
{
	csG_byte foo;
	switch (addr.kind) {
	case csC_temp:{
		int tmp = addr.u.tmp->num;
		int reg = (int)(long)csH_tablook(regtab, (void*)(long)tmp);
		if (!reg) {
			csH_tabinsert(regtab, (void*)(long)tmp, (void*)(long)(*statrreg));
			foo = (*statrreg)++;
			return foo;
		}
		foo = reg;
		break;
	}
	case csC_env: {
		csE_enventry e = addr.u.eval;
		VERIFY(e);
		//VERIFY(e->kind == csE_var);
		if (e->kind == csE_var) {
			csF_access access = e->u.var.access;
			VERIFY(access);
			if (access->kind == f_reg) {
				VERIFY(access->u.reg);
				foo = access->u.reg->num;
			} else if (access->kind == f_static) {
				foo = access->u.offset;
			} else {
				VERIFY(0);
			}
		} else {
			foo = 100;
		}
		break;
	}
	case csC_intconst: {
		foo = (*statrreg)++;
		csG_2byte bx = b_constregin_int_(addr.u.ival);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,head);
		(*offset)++;
		break;
	}
	case csC_strconst:{
		foo = (*statrreg)++;
		csG_2byte bx = b_constregin_str_(addr.u.str);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,head);
		(*offset)++;
		break;
	}
	case csC_boolconst:{
		foo = (*statrreg)++;
		csG_2byte bx =  b_constregin_bool_(addr.u.bval);
		csO_code code = csO_iABx(OP_LOADCONST,foo,bx);
		b_addcodelist(code,head);
		(*offset)++;
		break;
	}
	default:
		VERIFY(0);
	}
	return foo;
}

static void b_backpatch_(csH_table labtab,csL_list *head)
{
	VERIFY(head);
	csB_lablelist ll = NULL;
	list_for_each_entry(ll, head, next) {
		csO_code code = ll->code->code;
		csT_label lab = ll->lab;
		long l = (long)csH_tablook(labtab, lab);
		//VERIFY(l);
		switch (csO_iop(code)) {
		case OP_JMP:
			ll->code->code = csO_iAsBx(OP_JMP,0,l-ll->offset-1);
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
}
static csB_codelist b_addcodelist(csO_code code,csL_list *head)
{
	VERIFY(head);
	csB_codelist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, head);
	foo->code = code;
	return foo;
}

static void b_addlablelist(csB_codelist code,csT_label lab,int offset,csL_list *head)
{
	csB_lablelist foo = csU_malloc(sizeof(*foo));
	INIT_LIST_HEAD(&foo->next);
	foo->lab = lab;
	foo->code = code;
	foo->offset = offset;
	list_add_tail(&foo->next, head);
}

static csG_2byte b_constregin_str_(csG_string str)
{
	static csH_table b_strtab_;
	if (!b_strtab_) 
		b_strtab_ = csH_tabempty(csU_strhash,csU_strequal,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(b_strtab_, str);
	if (!off) {
		csH_tabinsert(b_strtab_, str, (void*)(long)b_const_offset_);
		b_regin_static_str_(str,b_const_offset_);
		return b_const_offset_++;
	}
	return off;
}

static csG_2byte b_constregin_int_(int num)
{
	static csH_table b_inttab_;
	if (!b_inttab_) 
		b_inttab_ = csH_tabempty(NULL,NULL,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(b_inttab_, (void*)(long)num);
	if (!off) {
		csH_tabinsert(b_inttab_, (void*)(long)num, (void*)(long)b_const_offset_);
		b_regin_const_int_(num,b_const_offset_);
		return b_const_offset_++;
	}
	return off;
}

static csG_2byte b_constregin_bool_(csG_bool boo)
{
	static csH_table b_booltab_;
	if (!b_booltab_) 
		b_booltab_ = csH_tabempty(NULL,NULL,NULL);
	csG_2byte off = (csG_2byte)(long)csH_tablook(b_booltab_, (void*)(long)boo);
	if (!off) {
		csH_tabinsert(b_booltab_, (void*)(long)boo, (void*)(long)b_const_offset_);
		b_regin_const_bool_(boo,b_const_offset_);
		return b_const_offset_++;
	}
	return off;
}


static void b_regin_static_int_(int val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_static_;
	foo->u.val.kind = f_int_;
	foo->u.val.u.intv = val;
	foo->offset = offset;
	foo->size = sizeof(int);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_const_int_(int val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_const_;
	foo->u.val.kind = f_int_;
	foo->u.val.u.intv = val;
	foo->offset = offset;
	foo->size = sizeof(int);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_static_bool_(csG_bool val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_static_;
	foo->u.val.kind = f_bool_;
	foo->u.val.u.boolv = val;
	foo->offset = offset;
	foo->size = sizeof(csG_bool);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_const_bool_(csG_bool val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_const_;
	foo->u.val.kind = f_bool_;
	foo->u.val.u.boolv = val;
	foo->offset = offset;
	foo->size = sizeof(csG_bool);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_static_str_(csG_string val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_static_;
	foo->u.val.kind = f_str_;
	foo->u.val.u.strv = val;
	foo->offset = offset;
	foo->size = strlen(val);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_const_str_(csG_string val,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_const_;
	foo->u.val.kind = f_str_;
	foo->u.val.u.strv = val;
	foo->offset = offset;
	foo->size = strlen(val);
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}

static void b_regin_proc_(csL_list *head,int size,int offset)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_prco_;
	foo->offset = offset;
	foo->size = size;
	foo->u.head = head;
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}
