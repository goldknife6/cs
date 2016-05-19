#include "csmemory.h"
#include "csformat.h"
#include "csutil.h"

#include <stdlib.h>
#include <string.h>


csO_value csM_const_regin[CONST_REGIN_MAX];	//const
csO_value csM_static_regin[STATIC_REGIN_MAX];	//static
csO_value csM_stack_regin[STACK_REGIN_MAX];	//stack
int csM_record_regin[RECORD_REGIN_MAX];	//record
csM_proc csM_proc_regin[PROC_REGIN_MAX];		//proc

int csM_pc;
int csM_fp;
int csM_sp;

static int m_rip_ = 0;
int csM_procp;

static void m_printheader_(csF_fmtheader header);
static void m_printregin_();

csO_code csM_nextins()
{
	csM_proc proc = csM_proc_regin[csM_procp];
	VERIFY(proc.ins);
	size_t size = proc.size;
	VERIFY(csM_pc < size);
	return proc.ins[csM_pc++];
}

void csM_change_proc(int proc)
{
	VERIFY(proc >= 0);
	csM_procp = proc;
}

void csM_push_record(int addr)
{
	VERIFY(addr >= 0);
	VERIFY(m_rip_ < RECORD_REGIN_MAX);
	csM_record_regin[m_rip_++] = addr;
}

int csM_pop_record()
{
	if(m_rip_ > 0)
		return csM_record_regin[--m_rip_];
	else {
		exit(0);
	}
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
	VERIFY(csM_sp < STACK_REGIN_MAX);
	csM_stack_regin[csM_sp++] = v;
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
	VERIFY(csM_fp + offset - 1 >= 0);
	v = csM_stack_regin[csM_fp + offset - 1];
	return v;
}

void csM_store_active_record_value(int offset,csO_value v)
{
	VERIFY(offset > 0);
	VERIFY(v.kind != csO_empty_);
	VERIFY(csM_fp + offset - 1 < STACK_REGIN_MAX);
	VERIFY(csM_fp + offset - 1 >= 0);
	csM_stack_regin[csM_fp + offset - 1] = v;
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
	//m_printheader_(header);
	staticsize = header.staticsize;
	constsize = header.constsize;
	procsize = header.procsize;
	char buffer[100];
	for (int i = 0; i < staticsize + constsize + procsize; i++) {
		csF_format fmt;
		int c = fread(&fmt, sizeof(fmt), 1, in);
		int offset = fmt.f_offset_;
		int size = fmt.f_size_;
		//csF_printfmt(fmt);
		VERIFY(c);
		csO_object obj = NULL;
		csO_value v;
		switch (fmt.f_kind_) {
		case f_static_:
			VERIFY(offset);
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
				obj = csS_string(buffer,size);
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
		case f_prco_: {
			VERIFY(offset);
			if (fmt.u.f_main_) {
				csM_procp = offset-1;
			}
			csM_proc_regin[offset-1].size = size/sizeof(csO_code);
			csO_code *code = malloc(size);
			csM_proc_regin[offset-1].ins = code;
			fread(code, size, 1, in);
			break;
		}
		case f_const_:
			VERIFY(offset);
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
				obj = csS_string(buffer,size);
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
