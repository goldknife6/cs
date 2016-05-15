#include "csmemory.h"
#include "csformat.h"
#include "csutil.h"

#include <stdlib.h>
#include <string.h>

static void csF_printfmt(csF_format fmt);
static void csF_printheader(csF_fmtheader header);

csM_regin csM_static_regin;
csM_regin csM_const_regin;
csM_regin csM_stack_regin;
csM_stackinf csM_stack_inf;
csM_procregin csM_proc_regin;


static csM_proc m_proc_(size_t size,csO_code *code)
{
	VERIFY(code);
	csM_proc foo = malloc(sizeof(*foo));
	foo->size = size;
	foo->code = code;
	return foo;
}

static void m_init_regin_(csM_regin *regin,size_t size)
{
	csO_object *obj = malloc(size * sizeof(csO_object));
	regin->obj = obj;
	memset(obj,0,size * sizeof(csO_object));
	regin->size = size;
}

static void m_init_int_(csM_stackinf *inf,size_t size)
{
	m_stackinf_ *foo = malloc(size * sizeof(m_stackinf_));
	inf->inf = foo;
	memset(foo,0,size * sizeof(m_stackinf_));
	inf->size = size;
}

void csM_load_bytecode(FILE *in)
{
	VERIFY(in);
	csF_fmtheader header;
	fread(&header, sizeof(header), 1, in);
	m_init_regin_(&csM_static_regin,header.staticsize);
	m_init_regin_(&csM_const_regin,header.constsize);
	m_init_regin_(&csM_stack_regin,10000);
	m_init_int_(&csM_stack_inf,10000);
	//csF_printheader(header);
	csM_proc *code = malloc(header.procsize * sizeof(csM_proc));
	csM_proc_regin.code = code;
	memset(code,0,header.procsize * sizeof(csM_proc));
	char buffer[1000];
	while (TRUE) {
		csF_format fmt;
		int c = fread(&fmt, sizeof(fmt), 1, in);
		if (!c)
			break;
		//csF_printfmt(fmt);
		int offset = fmt.f_offset_;
		switch (fmt.f_kind_) {
		case f_static_:{
			csO_object obj = NULL;
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int v;
				offset = fmt.f_offset_;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_int_object(v);
				break;
			}
			case f_str_:{
				VERIFY(fmt.f_size_ <= 1000);
				fread(buffer, fmt.f_size_, 1, in);
				obj = csO_string_object(buffer,fmt.f_size_);
				break;
			}
			case f_bool_:{
				csG_bool v;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_bool_object(v);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(!csM_static_regin.obj[offset-1]);
			csM_static_regin.obj[offset-1] = obj;
			break;
		}
		case f_prco_:{
			csO_code *code = malloc(fmt.f_size_);
			csM_proc proc = m_proc_(fmt.f_size_,code);
			fread(code, fmt.f_size_, 1, in);
			csM_proc_regin.code[offset-1] = proc;
			break;
		}
		case f_const_:{
			csO_object obj = NULL;
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int v;
				offset = fmt.f_offset_;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_int_object(v);
				break;
			}
			case f_str_:{
				VERIFY(fmt.f_size_ <= 1000);
				fread(buffer, fmt.f_size_, 1, in);
				obj = csO_string_object(buffer,fmt.f_size_);
				break;
			}
			case f_bool_:{
				csG_bool v;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_bool_object(v);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(!csM_const_regin.obj[offset-1]);
			csM_const_regin.obj[offset-1] = obj;
			break;
		}
		default:
			VERIFY(0);
		}
	}
}


static void csF_printheader(csF_fmtheader header)
{
	fprintf(stdout, "staticsize:%d\n",header.staticsize);
	fprintf(stdout, "constsize:%d\n",header.constsize);
	fprintf(stdout, "procsize:%d\n",header.procsize);
}


