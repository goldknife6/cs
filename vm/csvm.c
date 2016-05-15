#include "csvm.h"
#include "csmemory.h"

static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc);
static csO_object v_getobj_(csO_code code,csM_regin *cons,csM_regin *stat,csM_procregin *proc);

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
	v_exe_(&csM_const_regin,&csM_static_regin,&csM_proc_regin);
}

static csG_bool v_halt_;
static int v_pc_ = 0;
csO_object param; 
static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc)
{



    csM_proc _curproc = proc->code[0];
    VERIFY(_curproc);


	while (!v_halt_) {
		csO_code code = _curproc->code[v_pc_++];
		VERIFY(v_pc_ <= _curproc->size/4);
		switch (csO_iop(code)) {
		case OP_IFFALSE:{
			csO_object obj = v_getobj_(code,cons,stat,proc);
			VERIFY(obj->kind == csO_bool);
			if (!obj->u.bval) {
				v_pc_ += code.u.abx.bx;
			}
			break;
		}
		case OP_RETURN:
			VERIFY(0);
		case OP_IF:
			VERIFY(0);
		case OP_MOVE:
			VERIFY(0);
		case OP_LOADCONST:
			VERIFY(0);
		case OP_JMP:
			v_pc_ += code.u.abx.bx;
			break;
		case OP_PARAM:
			param = v_getobj_(code,cons,stat,proc);
			break;
		case OP_CALL:
			VERIFY(param->kind == csO_str);
			printf("%s\n", param->u.sval->s);
			break;
		default:
			VERIFY(0);
		}
	}
}

static csO_object v_getobj_(csO_code code,csM_regin *cons,csM_regin *stat,csM_procregin *proc)
{
	switch (csO_iop(code)) {
	case OP_IFFALSE:{
		csG_byte b = code.u.abx.b[1];
		VERIFY(b <= stat->size);
		return stat->obj[b-1];
		break;
	}
	case OP_RETURN:
	case OP_IF:
	case OP_MOVE:
	case OP_LOADCONST:
	case OP_JMP:
		VERIFY(0);
		break;
	case OP_PARAM: {
		csG_byte b = code.u.abc.b[1];
		VERIFY(b <= stat->size);
		return stat->obj[b-1];
		break;
	}
	default:
		VERIFY(0);
	}
}