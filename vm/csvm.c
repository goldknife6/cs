#include "csvm.h"
#include "csmemory.h"

static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc,csM_regin *stack,csM_stackinf *inf);
static void v_call_(csM_regin *cons,csM_regin *stat,csM_regin *stack,csM_procregin *proc,csM_stackinf *inf,csO_code code);
static void v_setstackobj(int offset,csM_regin *stack,csO_object obj);
static csO_object v_getstaticobj(int offset,csM_regin *stat);
static csO_object v_getstackobj(int offset,csM_regin *stack);

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
	v_exe_(&csM_const_regin,&csM_static_regin,&csM_proc_regin,&csM_stack_regin,&csM_stack_inf);
}

static csG_bool v_halt_;
static int v_pc_ = 0;
static int v_regsp_ = 0;
static int v_regsize;
static int v_isp_ = 0;
static void v_exe_(csM_regin *cons,csM_regin *stat,csM_procregin *proc,csM_regin *stack,csM_stackinf *inf)
{

    csM_proc _curproc = proc->code[1];
    VERIFY(_curproc);

	while (!v_halt_) {
		csO_code code = _curproc->code[v_pc_++];
		VERIFY(v_pc_ <= _curproc->size/4);
		switch (csO_iop(code)) {
		case OP_IFFALSE:{
			csO_object obj = v_getstackobj(csO_iA(code),stack);
			VERIFY(obj);
			VERIFY(obj->kind == csO_bool);
			if (!obj->u.bval) {
				v_pc_ += code.u.abx.bx;
			}
			v_regsize = csO_iA(code);
			break;
		}
		case OP_RETURN:
			VERIFY(0);
		case OP_IF:{
			csO_object obj = v_getstackobj(csO_iA(code),stack);
			VERIFY(obj);
			VERIFY(obj->kind == csO_bool);
			if (obj->u.bval) {
				v_pc_ += code.u.abx.bx;
			}
			v_regsize = csO_iA(code);
			break;
		}
		case OP_MOVE:{
			csO_object obj = v_getstaticobj(csO_iB(code),stack);
			v_setstackobj(csO_iA(code),stack,obj);
			break;
		}
		case OP_LOADCONST:{
			csO_object obj = v_getstaticobj(csO_iBx(code),cons);
			v_setstackobj(csO_iA(code),stack,obj);
			v_regsize = csO_iA(code);
			break;
		}
		case OP_LOADSTATIC:{
			csO_object obj = v_getstaticobj(csO_iBx(code),stat);
			v_setstackobj(csO_iA(code),stack,obj);
			v_regsize = csO_iA(code);
			break;
		}
		case OP_JMP:
			v_pc_ += code.u.abx.bx;
			VERIFY(v_pc_>= 0);
			v_regsize = csO_iA(code);
			break;
		case OP_PARAM:{
			break;
		}
		case OP_CALL:
			v_call_(cons,stat,stack,proc,inf,code);
			v_regsize = csO_iA(code);
			break;
		case OP_LT:{
			csO_object obj1 = v_getstaticobj(csO_iB(code),stack);
			csO_object obj2 = v_getstaticobj(csO_iC(code),stack);
			VERIFY(obj1);
			VERIFY(obj2);
			VERIFY(obj1->kind == obj2->kind);
			csO_object b;
			switch (obj1->kind) {
			case csO_int:{
				if (obj1->u.ival < obj2->u.ival)
					b = csO_bool_object(TRUE);
				else
					b = csO_bool_object(FALSE);
				break;
			}
			case csO_bool:
				VERIFY(0);
			case csO_str:
				VERIFY(0);
			default:
				VERIFY(0);
			}
			v_setstackobj(csO_iA(code),stack,b);
			v_regsize = csO_iA(code);
			break;
		}
		case OP_EQ:{
			csO_object obj1 = v_getstaticobj(csO_iB(code),stack);
			csO_object obj2 = v_getstaticobj(csO_iC(code),stack);
			VERIFY(obj1);
			VERIFY(obj2);
			VERIFY(obj1->kind == obj2->kind);
			csO_object b;
			switch (obj1->kind) {
			case csO_int:{
				if (obj1->u.ival == obj2->u.ival)
					b = csO_bool_object(TRUE);
				else
					b = csO_bool_object(FALSE);
				break;
			}
			case csO_bool:
				VERIFY(0);
			case csO_str:
				VERIFY(0);
			default:
				VERIFY(0);
			}
			v_setstackobj(csO_iA(code),stack,b);
			v_regsize = csO_iA(code);
			break;
		}
		case OP_ADD:{
			csO_object obj1 = v_getstaticobj(csO_iB(code),stack);
			csO_object obj2 = v_getstaticobj(csO_iC(code),stack);
			VERIFY(obj1);
			VERIFY(obj2);
			VERIFY(obj1->kind == obj2->kind);
			csO_object b;
			switch (obj1->kind) {
			case csO_int:{
				b = csO_int_object(obj1->u.ival + obj2->u.ival);
				break;
			}
			case csO_bool:
				VERIFY(0);
			case csO_str:
				VERIFY(0);
			default:
				VERIFY(0);
			}
			v_setstackobj(csO_iA(code),stack,b);
			v_regsize = csO_iA(code);
			break;
		}
		default:
			VERIFY(0);
		}
	}
}

static csO_object v_getstackobj(int offset,csM_regin *stack)
{
	VERIFY(offset+v_regsp_ <= stack->size);
	return stack->obj[v_regsp_+offset-1];
}

static csO_object v_getstaticobj(int offset,csM_regin *stat)
{
	VERIFY(offset <= stat->size);
	return stat->obj[offset-1];
}

static void v_setstackobj(int offset,csM_regin *stack,csO_object obj)
{
	VERIFY(offset+v_regsp_ <= stack->size);
	stack->obj[v_regsp_+offset-1] = obj;
}

static void v_call_(csM_regin *cons,csM_regin *stat,csM_regin *stack,csM_procregin *proc,csM_stackinf *inf,csO_code code)
{
	csG_byte ret = csO_iA(code);
	csG_byte offset = csO_iB(code);
	csG_byte num = csO_iC(code);
	switch (offset) {
	case 1: {
		int pn = csO_iC(code);//inf->inf[v_isp_].nparam;
		v_regsp_ = v_regsp_ + v_regsize - pn + 1;
		//printf("% d %d %d\n",v_regsp_ ,v_regsize,pn);
		for (int i = 0; i < pn ; i++){
			csO_object obj = v_getstackobj(i,stack);
			VERIFY(obj);
			csO_pobject(obj);
		}
		printf("\n");
		v_regsp_ = v_regsp_ - v_regsize + pn - 1;
		break;
	}
	default:
		VERIFY(0);	
	}
}