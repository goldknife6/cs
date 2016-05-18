#include "csmemory.h"
#include "csformat.h"
#include "csutil.h"

#include <stdlib.h>
#include <string.h>


csO_value csM_const_regin[CONST_REGIN_MAX];	//const
csO_value csM_static_regin[STATIC_REGIN_MAX];	//static
csO_value csM_stack_regin[STACK_REGIN_MAX];	//stack
csM_record csM_record_regin[RECORD_REGIN_MAX];	//record
csM_proc csM_proc_regin[PROC_REGIN_MAX];		//proc

int csM_pc;
int csM_fp;
int csM_sp;

static int m_curproc_;

static void m_printheader_(csF_fmtheader header);
static void m_printregin_();

csO_code csM_nextins()
{
	csM_proc proc = csM_proc_regin[m_curproc_];
	VERIFY(proc.ins);
	size_t size = proc.size;
	VERIFY(csM_pc < size);
	return proc.ins[csM_pc++];
}

csO_value csM_const_value(int offset)
{
	csO_value v;
	VERIFY(offset > 0);
	VERIFY(offset <= CONST_REGIN_MAX);
	v = csM_const_regin[offset-1];
	return v;
}

csO_value csM_static_value(int offset)
{
	csO_value v;
	VERIFY(offset > 0);
	VERIFY(offset <= CONST_REGIN_MAX);
	v = csM_static_regin[offset-1];
	return v;
}

void csM_store_static_value(int offset,csO_value v)
{
	VERIFY(offset > 0);
	VERIFY(offset <= CONST_REGIN_MAX);
	csM_static_regin[offset-1] = v;
}

void csM_push_object(csO_object obj)
{
	VERIFY(obj);
	csO_value v;
	v.kind = csO_obj_;
	v.u.gc = obj;
	csM_push_value(v);
}

csO_object csM_pop_object(void)
{
	csO_object obj = NULL;
	csO_value v = csM_pop_value();
	VERIFY(v.kind == csO_obj_);
	obj = v.u.gc;
	return obj;
}

csO_value csM_pop_value(void)
{
	csO_value v;
	VERIFY(csM_sp > 0);
	v = csM_stack_regin[--csM_sp];
	return v;
}

void csM_push_value(csO_value v)
{
	VERIFY(v.kind != csO_empty_);
	csM_stack_regin[csM_sp++] = v;
	VERIFY(csM_sp < STACK_REGIN_MAX);
}

void csM_push_address(int addr)
{
	VERIFY(addr > 0);
	csO_value v;
	v.kind = csO_addr_;
	v.u.addr = addr;
	csM_push_value(v);
}

int csM_pop_address(void)
{
	VERIFY(csM_sp > 0);
	csO_value v = csM_pop_value();
	VERIFY(v.kind == csO_addr_);
	int addr = v.u.addr;
	VERIFY(addr > 0);
	return addr;
}

csO_value csM_active_record_value(int offset)
{
	csO_value v;
	VERIFY(offset > 0);
	VERIFY(csM_fp + offset - 1 < STACK_REGIN_MAX);
	v = csM_stack_regin[csM_fp + offset - 1];
	return v;
}

csO_value csO_int_value(int val)
{
	csO_value v;
	v.kind = csO_int_;
	v.u.ival = val;
	return v;
}

csO_value csO_bool_value(csG_bool val)
{
	csO_value v;
	v.kind = csO_bool_;
	v.u.bval = val;
	return v;
}

csO_value csO_object_value(csO_object obj)
{
	csO_value v;
	v.kind = csO_obj_;
	v.u.gc = obj;
	VERIFY(obj);
	return v;
}

void csM_load_bytecode(FILE *in)
{
	VERIFY(in);
	csF_fmtheader header;
	int staticsize,constsize,procsize;
	fread(&header, sizeof(header), 1, in);
	//read header info
	VERIFY(header.id == FORMATID);
	m_printheader_(header);
	staticsize = header.staticsize;
	constsize = header.constsize;
	procsize = header.procsize;
	char buffer[100];
	for (int i = 0; i < staticsize + constsize + procsize; i++) {
		csF_format fmt;
		int c = fread(&fmt, sizeof(fmt), 1, in);
		int offset = fmt.f_offset_;
		int size = fmt.f_size_;
		VERIFY(offset);
		VERIFY(c);
		csO_object obj = NULL;
		csO_value v;
		switch (fmt.f_kind_) {
		case f_static_:
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int ival;
				fread(&ival, size, 1, in);
				v = csO_int_value(ival);
				break;
			}
			case f_str_:
				VERIFY(size <= 100);
				fread(buffer, size, 1, in);
				obj = csO_string_object(buffer,size);
				v = csO_object_value(obj);
				break;
			case f_bool_:{
				csG_bool bval;
				fread(&bval, size, 1, in);
				v = csO_bool_value(bval);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(csM_static_regin[offset-1].kind == csO_empty_);
			csM_static_regin[offset-1] = v;
			break;
		case f_prco_:{
			csM_proc_regin[offset-1].size = size/sizeof(csO_code);
			csO_code *code = malloc(size);
			csM_proc_regin[offset-1].ins = code;
			fread(code, size, 1, in);
			break;
		}
		case f_const_:
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int ival;
				fread(&ival, size, 1, in);
				v = csO_int_value(ival);
				break;
			}
			case f_str_:
				VERIFY(size <= 100);
				fread(buffer, size, 1, in);
				obj = csO_string_object(buffer,size);
				v = csO_object_value(obj);
				break;
			case f_bool_:{
				csG_bool bval;
				fread(&bval, size, 1, in);
				v = csO_bool_value(bval);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(csM_const_regin[offset-1].kind == csO_empty_);
			csM_const_regin[offset-1] = v;
			break;
		default:
			VERIFY(0);
		}
	}
}



static void m_printheader_(csF_fmtheader header)
{
	fprintf(stdout, "staticsize:%d\n",header.staticsize);
	fprintf(stdout, "constsize:%d\n",header.constsize);
	fprintf(stdout, "procsize:%d\n",header.procsize);
}

static void m_printregin_()
{

}
/*
static char *csO_opnames[NUM_OPCODES+1] = {
	"LOADC",
	"LOADS",
	"LOADL",
	"LOADA",
	"STORES",
	"STOREL",
	"MARK",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"EQ",
	"NEQ",
	"LT",
	"LQ",
	"GT",
	"GQ",
	"FJP",
	"TJP",
	"UJP",
	"CUP",
	"CBP",
	"RET",
	"OR",
	"AND",
	"MINUS",
	"NOT",
	NULL	
};

void csO_printcode(csO_code code)
{
	fprintf(stderr, "%s %d\n", csO_opnames[csO_codeop(code)],code.d);
}*/