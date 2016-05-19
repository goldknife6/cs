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
static csO_value v_eq_(csO_value v1,csO_value v2);
static csO_value v_gt_(csO_value v1,csO_value v2);
static void v_fjp_(csO_value v,int offset);
static void v_tjp_(csO_value v,int offset);
static void v_mark_();

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
			VERIFY(v.kind != csO_empty_);
			csM_push_value(v);
			break;
		case OP_LOADS:
			v = csM_static_value(csO_codeval(ins));
			VERIFY(v.kind != csO_empty_);
			csM_push_value(v);
			break;
		case OP_LOADL:
			v = csM_active_record_value(csO_codeval(ins));
			VERIFY(v.kind != csO_empty_);
			csM_push_value(v);
			break;
		case OP_LOADA:
			csM_push_address(csO_codeval(ins));
			break;
		case OP_STORES:
			v = csM_pop_value();
			VERIFY(v.kind != csO_empty_);
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
			csO_value v1 = csM_pop_value();
			csO_value v2 = csM_pop_value();
			csM_push_value(v_add_(v1,v2));
			break;
		}
		case OP_SUB: {
			csO_value v2 = csM_pop_value();
			csO_value v1 = csM_pop_value();
			csM_push_value(v_sub_(v1,v2));
			break;
		}
		case OP_MUL:
			VERIFY(0);
		case OP_DIV:
			VERIFY(0);
		case OP_EQ:{
			csO_value v1 = csM_pop_value();
			csO_value v2 = csM_pop_value();
			csM_push_value(v_eq_(v1,v2));
			break;
		}
		case OP_NEQ:
			VERIFY(0);
		case OP_LT:
			VERIFY(0);
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
			VERIFY(0);
		case OP_RET:
			csM_change_proc(csM_pop_record());
			csM_pc = csM_pop_record();
			csM_sp = csM_fp;
			csM_fp = csM_pop_record();
			//fprintf(stderr, "OP_RET pop pc:%d sp:%d\n", csM_pc,csM_sp);
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
			v = csM_pop_value();
			//VERIFY(0);
			csM_sp = csM_fp;
			csM_push_value(v);
			break;
		default:
			VERIFY(0);
		}
	}
}

static csO_value v_add_(csO_value v1,csO_value v2)
{
	csO_value v;
	if(v1.kind != v2.kind) {
		fprintf(stderr, "%d %d\n",v1.kind,v2.kind );
		VERIFY(0);
	}
	if (v1.kind == csO_int_) {
		v = csO_int_value(v1.u.ival + v2.u.ival);
		//printf("%d\n", v.u.ival);
	} else 
		VERIFY(0);
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

static csO_value v_eq_(csO_value v1,csO_value v2)
{
	csO_value v;
	VERIFY(v1.kind == v2.kind);
	if (v1.kind == csO_int_) {
		v = csO_bool_value(v1.u.ival == v2.u.ival);
	} else 
		VERIFY(0);
	return v;
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