#include "csvm.h"
#include "csmemory.h"


extern int csM_pc;
extern int csM_fp;
extern int csM_sp;
extern int csM_procp;

static csG_bool v_halt_ = FALSE;
static void v_mainloop_();
static csO_code v_curins();

static csO_value v_add_(csO_value v1,csO_value v2);
static csO_value v_sub_(csO_value v1,csO_value v2);
static csO_value v_mul_(csO_value v1,csO_value v2);
static csO_value v_eq_(csO_value v1,csO_value v2);
static csO_value v_neq_(csO_value v1,csO_value v2);
static csO_value v_gt_(csO_value v1,csO_value v2);
static void v_fjp_(csO_value v,int offset);
static void v_tjp_(csO_value v,int offset);
static void v_mark_();
static void v_cbp_(int cnum);
static void v_ret_();
static void v_lt_();
static void v_prv_();

void main(int argc, char *argv[])
{
	csG_string filename;
	if (argc != 2) 
		fprintf(stderr,"usage: %s <filename>",argv[0]);
	filename = argv[1];
	FILE *ins = fopen(filename,"r");
	if (ins == NULL) 
		fprintf(stderr,"File %s not found",filename);


	csM_load_bytecode(ins);

	v_mainloop_();
}

static void v_mainloop_()
{
	while (!v_halt_) {
		csO_code ins = csM_nextins();
		csO_object obj = NULL;
		csO_value v;
		//csO_printcode(ins);
		switch (csO_codeop(ins)) {
		case OP_LOADC:
			v = csM_const_value(csO_codeval(ins));
			VERIFY(v.kind != csO_null_);
			csM_push_value(v);
			break;
		case OP_LOADN:
			csM_push_value(csM_null_value());
			break;
		case OP_LOADS:
			v = csM_static_value(csO_codeval(ins));
			VERIFY(v.kind != csO_null_);
			csM_push_value(v);
			break;
		case OP_LOADL:
			v = csM_active_record_value(csO_codeval(ins));
			//VERIFY(v.kind != csO_null_);
			csM_push_value(v);
			break;
		case OP_LOADA:
			csM_push_address(csO_codeval(ins));
			break;
		case OP_STORES:
			v = csM_pop_value();
			VERIFY(v.kind != csO_null_);
			csM_store_static_value(csM_pop_address(),v);
			break;
		case OP_STOREL:
			v = csM_pop_value();
			csM_store_active_record_value(csM_pop_address(),v);
			//VERIFY(0);
			break;
		case OP_MARK:
			v_mark_();
			break;
		case OP_ADD:{
			csO_value v2 = csM_pop_value();
			csO_value v1 = csM_pop_value();
			csM_push_value(v_add_(v1,v2));
			break;
		}
		case OP_SUB: {
			csO_value v2 = csM_pop_value();
			csO_value v1 = csM_pop_value();
			csM_push_value(v_sub_(v1,v2));
			break;
		}
		case OP_MUL:{
			csO_value v2 = csM_pop_value();
			csO_value v1 = csM_pop_value();
			csM_push_value(v_mul_(v1,v2));
			break;
		}
		case OP_DIV:
			VERIFY(0);
		case OP_EQ:{
			csO_value v1 = csM_pop_value();
			csO_value v2 = csM_pop_value();
			csM_push_value(v_eq_(v1,v2));
			break;
		}
		case OP_NEQ:{
			csO_value v1 = csM_pop_value();
			csO_value v2 = csM_pop_value();
			csM_push_value(v_neq_(v1,v2));
			break;
		}
		case OP_LT:
			v_lt_();
			break;
		case OP_LQ:
			VERIFY(0);
		case OP_GT:{
			csO_value v2 = csM_pop_value();
			csO_value v1 = csM_pop_value();
			csM_push_value(v_gt_(v1,v2));
			break;
		}
		case OP_GQ:
			VERIFY(0);
		case OP_FJP:
			v = csM_pop_value();
			v_fjp_(v,csO_codeval(ins));
			break;
		case OP_TJP:
			v = csM_pop_value();
			v_tjp_(v,csO_codeval(ins));
			break;
		case OP_UJP:
			csM_pc += csO_codeval(ins);
			break;
		case OP_CUP:
			//fprintf(stderr," %d \n",csM_sp);
			csM_fp = csM_pop_record();
			csM_push_record(csM_pc);
			csM_push_record(csM_procp);
			//fprintf(stderr, "OP_CUP push pc:%d proc:%d\n", csM_pc,csM_procp);
			csM_pc = 0;
			csM_change_proc(csO_codeval(ins)-1);
			break;
		case OP_CBP:
			v_cbp_(csO_codeval(ins));
			break;
		case OP_RET:
			v_ret_();
			break;
		case OP_OR:
			VERIFY(0);
		case OP_AND:
			VERIFY(0);
		case OP_MINUS:
			VERIFY(0);
		case OP_NOT:
			VERIFY(0);
		case OP_SSP:
			csM_sp -= csO_codeval(ins);
			break;
		case OP_PRV:
			v_prv_();
			break;
		default:
			VERIFY(0);
		}
	}
}

static void v_prv_()
{
	csO_value v = csM_pop_value();;
	csM_sp = csM_fp;
	csM_push_value(v);
}

static csO_value v_add_(csO_value v1,csO_value v2)
{
	csO_value v;
	if(v1.kind != v2.kind) {
		fprintf(stderr, "%d %d\n",v1.kind,v2.kind);
		VERIFY(0);
	}
	if (v1.kind == csO_int_) {
		v = csO_int_value(v1.u.ival + v2.u.ival);
	} else if (v1.kind == csO_obj_) {
		csO_object obj = csO_string_object_add(v1.u.gc,v2.u.gc);
		v = csO_object_value(obj);
	} else {
		VERIFY(0);
	}
	return v;
}

static csO_value v_sub_(csO_value v1,csO_value v2)
{
	csO_value v;
	VERIFY(v1.kind == v2.kind);
	if (v1.kind == csO_int_) {
		v = csO_int_value(v1.u.ival - v2.u.ival);
	} else 
		VERIFY(0);
	return v;
}

static csO_value v_mul_(csO_value v1,csO_value v2)
{
	csO_value v;
	VERIFY(v1.kind == v2.kind);
	if (v1.kind == csO_int_) {
		v = csO_int_value(v1.u.ival * v2.u.ival);
	} else 
		VERIFY(0);
	return v;
}

static csO_value v_eq_(csO_value v1,csO_value v2)
{
	csO_value v;
	if (v1.kind == csO_obj_ && v2.kind == csO_null_)
		v = csO_bool_value(FALSE);
	if (v2.kind == csO_obj_ && v1.kind == csO_null_)
		v = csO_bool_value(FALSE);
	else if (v1.kind == csO_int_) {
		v = csO_bool_value(v1.u.ival == v2.u.ival);
	} else if (v1.kind == csO_null_) {
		v = csO_bool_value(TRUE);
	} else
		VERIFY(0);
	return v;
}

static csO_value v_neq_(csO_value v1,csO_value v2)
{
	csO_value v;
	if (v1.kind == csO_obj_ && v2.kind == csO_null_)
		v = csO_bool_value(TRUE);
	if (v2.kind == csO_obj_ && v1.kind == csO_null_)
		v = csO_bool_value(TRUE);
	else if (v1.kind == csO_int_) {
		v = csO_bool_value(v1.u.ival != v2.u.ival);
	} else if (v1.kind == csO_null_) {
		v = csO_bool_value(FALSE);
	} else
		VERIFY(0);
	return v;
}

static void v_lt_()
{
	csO_value v;
	csO_value v2 = csM_pop_value();
	csO_value v1 = csM_pop_value();
	VERIFY(v1.kind == v2.kind);
	if (v1.kind == csO_int_) {
		v = csO_bool_value(v1.u.ival < v2.u.ival);
	} else 
		VERIFY(0);
	csM_push_value(v);
}

static csO_value v_gt_(csO_value v1,csO_value v2)
{
	csO_value v;
	VERIFY(v1.kind == v2.kind);
	if (v1.kind == csO_int_) {
		v = csO_bool_value(v1.u.ival > v2.u.ival);
	} else 
		VERIFY(0);
	return v;
}

static void v_fjp_(csO_value v,int offset)
{
	VERIFY(v.kind == csO_bool_);
	if (!v.u.bval)
		csM_pc += offset;
}

static void v_tjp_(csO_value v,int offset)
{
	VERIFY(v.kind == csO_bool_);
	if (v.u.bval)
		csM_pc += offset;
}

static void v_mark_()
{
	csM_push_record(csM_fp);
	csM_push_record(csM_sp);
}

static void v_ret_()
{
	csM_change_proc(csM_pop_record());
	csM_pc = csM_pop_record();
	csM_sp = csM_fp;
	csM_fp = csM_pop_record();
}

static void v_cbp_(int cnum)
{
	VERIFY(cnum > 0);
	csO_object obj = NULL;
	csO_value v;

	csM_fp = csM_pop_record();
	csM_push_record(csM_pc);
	csM_push_record(csM_procp);
	static char buffer[100];
	switch (cnum) {
	case 1:
		v = csM_pop_value();
		VERIFY(v.kind == csO_int_);
		printf("%d", v.u.ival);
		break;
	case 2:
		obj = csM_pop_object();
		csO_pobject(obj);
		//printf("\n");
		break;
	case 3:
		obj = csM_pop_object();
		VERIFY(obj->kind == csO_string_);
		obj = csF_file(obj->u.sval.s);
		if (obj)
			csM_push_object(obj);
		else
			csM_push_value(csM_null_value());
		v_prv_();
		break;
	case 4:{
		csO_object file,string;
		file = csM_pop_object();
		VERIFY(file->kind == csO_file_);
		v = csM_pop_value();
		VERIFY(v.kind == csO_int_);
		csO_value vs = csM_pop_value();
		if (vs.kind == csO_obj_) {
			string = vs.u.gc;
			VERIFY(string);
			char *s = csS_string_extend(string,v.u.ival);
			int a = fread(s,1, v.u.ival, file->u.fval.f);
			v.u.ival = a;
			csM_push_value(v);
			v_prv_();
		} else VERIFY(0);
		break;
	}
	default:
		VERIFY(0);
	}
	v_ret_();
}