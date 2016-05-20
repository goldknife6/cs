#include "csopcodes.h"
#include "csparser.h"
#include "csenv.h"
#include "cshash.h"
#include "csutil.h"
#include "csbytec.h"

static csO_4byte b_const_offset_ = 1;



static void b_regin_(csC_fraglist fraglist);
static void b_bytecode_(FILE *outs);
static csL_list b_regin_head_ = LIST_HEAD_INIT(b_regin_head_);

static void b_regin_static_bool_(csG_bool val,int offset);
static void b_regin_static_int_(int val,int offset);
static void b_regin_static_str_(csG_string val,int offset);
static void b_regin_const_bool_(csG_bool val,int offset);
static void b_regin_const_int_(int val,int offset);
static void b_regin_const_str_(csG_string val,int offset);


static void b_regin_proc_(csL_list *head,int size,int offset,csG_bool main);
static int b_genproc_(csC_quadlist body,csL_list *head);
static csB_codelist b_addcodelist(csO_code code,csL_list *head);
static void b_addlablelist(csB_codelist code,csT_label lab,int offset,csL_list *head);

static csO_4byte b_constregin_int_(int num);
static csO_4byte b_constregin_bool_(csG_bool boo);
static csO_4byte b_constregin_str_(csG_string str);
static void b_backpatch_(csH_table labtab,csL_list *head);

extern void csC_printcode();
void csB_genbytecode()
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	csC_fraglist fraglist = c_declist_(val,type,list);

	if (csG_error || !fraglist || list_empty(fraglist) )
		return;

	csC_printcode();
	b_regin_(fraglist);
	b_bytecode_(outs);
}

static void b_regin_(csC_fraglist fraglist)
{
	csC_frag o_pos_ = NULL;
	list_for_each_entry(o_pos_, fraglist, next) {
		switch (o_pos_->kind) {
		case csC_procfrag: {
			csL_list* prochead = NULL;
			prochead = csU_malloc(sizeof(*prochead));
			INIT_LIST_HEAD(prochead);
			csF_frame frame = o_pos_->u.proc.frame;
			csC_quadlist body = o_pos_->u.proc.body;
			VERIFY(body);
			VERIFY(frame);
			csS_symbol name = o_pos_->name;
			VERIFY(name);
			csG_bool main = FALSE;
			if (csS_compare(name, csS_mksymbol("main")))
				main = TRUE;
			int count = b_genproc_(body,prochead);
			b_regin_proc_(prochead,count*sizeof(csO_code),frame->offset,main);
			break;
		}
		case csC_strfrag: {
			int offset = o_pos_->offset;
			VERIFY(offset);
			b_regin_static_str_(o_pos_->u.strv,offset);
			break;
		}
		case csC_intfrag:{
			int offset = o_pos_->offset;
			VERIFY(offset);
			b_regin_static_int_(o_pos_->u.intv,offset);

			break;
		}
		case csC_boolfrag:{
			int offset = o_pos_->offset;
			VERIFY(offset);
			b_regin_static_bool_(o_pos_->u.boolv,offset);
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
			//csF_printfmt(format);
			break;
		}
		case f_prco_:{
			format = csF_proc(o_pos_->size,o_pos_->offset,o_pos_->u.fun.main);
			fwrite(&format, sizeof(format), 1, outs);
			VERIFY(o_pos_->u.fun.head);
			csB_codelist p_code_;
			csO_code code;
			list_for_each_entry(p_code_, o_pos_->u.fun.head, next) {
				code = p_code_->code;
				csO_printcode(code);
				fwrite(&code, sizeof(code), 1, outs);
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
			//csF_printfmt(format);
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

	csO_code code;
	
	list_for_each_entry(p_pos_, body, next) {
		csC_address addr = p_pos_->arg;
		switch (p_pos_->kind) {
		case csC_entry:
			
			continue;
			break;
		case csC_mark:
			code = csO_mkcode(OP_MARK,0);
			b_addcodelist(code,head);
			break;
		case csC_cup:
			if (addr.kind == csC_env) {
				csE_enventry eval = addr.u.eval;
				VERIFY(eval);
				VERIFY(eval->kind == csE_fun);
				csF_frame f = eval->u.fun.frame;
				VERIFY(f);
				code = csO_mkcode(OP_CUP,f->offset);
			} else VERIFY(0);
			b_addcodelist(code,head);
			break;
		case csC_cbp:
			if (addr.kind == csC_env) {
				csE_enventry eval = addr.u.eval;
				VERIFY(eval);
				VERIFY(eval->kind == csE_fun);
				csF_frame f = eval->u.fun.frame;
				VERIFY(f);
				code = csO_mkcode(OP_CBP,f->offset);
			} else VERIFY(0);
			b_addcodelist(code,head);
			break;
		case csC_ret:
			code = csO_mkcode(OP_RET,0);
			b_addcodelist(code,head);
			break;
		case csC_load:
			switch (addr.kind) {
			case csC_intconst:
				code = csO_mkcode(OP_LOADC,b_constregin_int_(addr.u.ival));
				break;
			case csC_strconst:
				code = csO_mkcode(OP_LOADC,b_constregin_str_(addr.u.str));
				break;
			case csC_boolconst:
				code = csO_mkcode(OP_LOADC,b_constregin_bool_(addr.u.bval));
				break;
			case csC_env: {
				csE_enventry eval = addr.u.eval;
				VERIFY(eval);
				VERIFY(eval->kind == csE_var);
				csF_access acc = eval->u.var.access;
				VERIFY(acc);
				if (acc->kind == f_frame){
					code = csO_mkcode(OP_LOADL,acc->offset);
				} else if (acc->kind == f_static) {
					code = csO_mkcode(OP_LOADS,acc->offset);
				} else VERIFY(0);
				break;
			}
			default:
				VERIFY(0);
			}
			b_addcodelist(code,head);
			break;
		case csC_loadnull:
			code = csO_mkcode(OP_LOADN,0);
			b_addcodelist(code,head);
			break;
		case csC_loadaddr:
			if (addr.kind == csC_env) {
				csE_enventry eval = addr.u.eval;
				VERIFY(eval);
				VERIFY(eval->kind == csE_var);
				csF_access acc = eval->u.var.access;
				VERIFY(acc);
				if (acc->kind == f_frame){
					code = csO_mkcode(OP_LOADA,acc->offset);
				} else if (acc->kind == f_static) {
					code = csO_mkcode(OP_LOADA,acc->offset);
				} else VERIFY(0);
			} else VERIFY(0);
			b_addcodelist(code,head);
			break;
		case csC_storel:
			code = csO_mkcode(OP_STOREL,0);
			b_addcodelist(code,head);
			break;
		case csC_stores:
			code = csO_mkcode(OP_STORES,0);
			b_addcodelist(code,head);
			break;
		case csC_fjp:{
			csT_label lab = addr.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			code = csO_mkcode(OP_FJP,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_tjp:{
			csT_label lab = addr.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			code = csO_mkcode(OP_TJP,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_ujp:{
			csT_label lab = addr.u.lab;
			long l = (long)csH_tablook(labtab, lab);
			code = csO_mkcode(OP_UJP,l-p_count_-1);
			csB_codelist list = b_addcodelist(code,head);
			if (!l) {
				b_addlablelist(list,lab,p_count_,&labhead);
			}
			break;
		}
		case csC_lab:{
			csT_label lab = addr.u.lab;
			VERIFY(lab);
			VERIFY(!csH_tablook(labtab, lab));
			csH_tabinsert(labtab, lab, (void*)(long)p_count_);
			continue;
		}
		case csC_add:
			code = csO_mkcode(OP_ADD,0);
			b_addcodelist(code,head);
			break;
		case csC_sub:
			code = csO_mkcode(OP_SUB,0);
			b_addcodelist(code,head);
			break;
		case csC_minus:
			code = csO_mkcode(OP_MINUS,0);
			b_addcodelist(code,head);
			break;
		case csC_eq:
			code = csO_mkcode(OP_EQ,0);
			b_addcodelist(code,head);
			break;
		case csC_gt:
			code = csO_mkcode(OP_GT,0);
			b_addcodelist(code,head);
			break;
		case csC_ssp:
			VERIFY(addr.kind == csC_intconst);
			code = csO_mkcode(OP_SSP,addr.u.ival);
			b_addcodelist(code,head);
			break;
		case csC_prv:
			code = csO_mkcode(OP_PRV,0);
			b_addcodelist(code,head);
			break;
		case csC_mul:
			code = csO_mkcode(OP_MUL,0);
			b_addcodelist(code,head);
			break;
		case csC_neq:
			code = csO_mkcode(OP_NEQ,0);
			b_addcodelist(code,head);
			break;
		case csC_lt:
			code = csO_mkcode(OP_LT,0);
			b_addcodelist(code,head);
			break;
		default:
			VERIFY(0);
		}
		p_count_++;
	}
	b_backpatch_(labtab,&labhead);
	return p_count_;
}

static void b_backpatch_(csH_table labtab,csL_list *head)
{
	VERIFY(head);
	csB_lablelist ll = NULL;
	list_for_each_entry(ll, head, next) {
		csO_code code = ll->code->code;
		csT_label lab = ll->lab;
		long l = (long)csH_tablook(labtab, lab);
		switch (csO_codeop(code)) {
		case OP_UJP:
			ll->code->code = csO_mkcode(OP_UJP,l-ll->offset-1);
			break;
		case OP_TJP:
			ll->code->code = csO_mkcode(OP_TJP,l-ll->offset-1);
			break;
		case OP_FJP:
			ll->code->code = csO_mkcode(OP_FJP,l-ll->offset-1);
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

static csO_4byte b_constregin_str_(csG_string str)
{
	static csH_table b_strtab_;
	if (!b_strtab_) 
		b_strtab_ = csH_tabempty(csU_strhash,csU_strequal,NULL);
	csO_4byte off = (csO_4byte)(long)csH_tablook(b_strtab_, str);
	if (!off) {
		csH_tabinsert(b_strtab_, str, (void*)(long)b_const_offset_);
		b_regin_const_str_(str,b_const_offset_);
		return b_const_offset_++;
	}
	return off;
}

static csO_4byte b_constregin_int_(int num)
{
	static csH_table b_inttab_;
	if (!b_inttab_) 
		b_inttab_ = csH_tabempty(NULL,NULL,NULL);
	csO_4byte off = (csO_4byte)(long)csH_tablook(b_inttab_, (void*)(long)num);
	if (!off) {
		csH_tabinsert(b_inttab_, (void*)(long)num, (void*)(long)b_const_offset_);
		b_regin_const_int_(num,b_const_offset_);

		return b_const_offset_++;
	}
	return off;
}

static csO_4byte b_constregin_bool_(csG_bool boo)
{
	static csH_table b_booltab_;
	if (!b_booltab_) 
		b_booltab_ = csH_tabempty(NULL,NULL,NULL);
	csO_4byte off = (csO_4byte)(long)csH_tablook(b_booltab_, (void*)(long)boo);
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

static void b_regin_proc_(csL_list *head,int size,int offset,csG_bool main)
{
	VERIFY(offset);
	csB_regin foo = csU_malloc(sizeof(*foo));
	foo->kind = f_prco_;
	foo->offset = offset;
	foo->size = size;
	foo->u.fun.head = head;
	foo->u.fun.main = main;
	INIT_LIST_HEAD(&foo->next);
	list_add_tail(&foo->next, &b_regin_head_);
}
