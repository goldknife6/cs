#include "csmemory.h"
#include "csformat.h"
#include "csutil.h"

#include <stdlib.h>
#include <string.h>



csO_object csM_const_regin[CONST_REGIN_MAX];	//const
csO_object csM_static_regin[STATIC_REGIN_MAX];	//static
csO_object csM_stack_regin[STACK_REGIN_MAX];	//stack
csM_record csM_record_regin[RECORD_REGIN_MAX];	//record
csM_proc csM_proc_regin[PROC_REGIN_MAX];		//proc


static void m_printheader_(csF_fmtheader header);
static void m_printregin_();



void csM_load_bytecode(FILE *in)
{
	VERIFY(in);
	csF_fmtheader header;
	int staticsize,constsize,procsize;
	fread(&header, sizeof(header), 1, in);
	//read header info
	VERIFY(header.id == FORMATID);
	csF_printheader(header);
	staticsize = header.staticsize;
	constsize = header.constsize;
	procsize = header.procsize;
	char buffer[100];
	for (int i = 0; i < staticsize + constsize + procsize; i++) {
		csF_format fmt;
		int c = fread(&fmt, sizeof(fmt), 1, in);
		int offset = fmt.f_offset_;
		VERIFY(offset);
		VERIFY(c);
		csO_object obj = NULL;
		switch (fmt.f_kind_) {
		case f_static_:
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int v;
				offset = fmt.f_offset_;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_int_object(v);
				break;
			}
			case f_str_:
				VERIFY(fmt.f_size_ <= 100);
				fread(buffer, fmt.f_size_, 1, in);
				obj = csO_string_object(buffer,fmt.f_size_);
				break;
			case f_bool_:{
				csG_bool v;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_bool_object(v);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(!csM_static_regin[offset-1]);
			csM_static_regin[offset-1] = obj;
			break;
		case f_prco_:
			VERIFY(0);
			break;
		case f_const_:
			switch (fmt.u.f_valkind_) {
			case f_int_:{
				int v;
				offset = fmt.f_offset_;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_int_object(v);
				break;
			}
			case f_str_:
				VERIFY(fmt.f_size_ <= 100);
				fread(buffer, fmt.f_size_, 1, in);
				obj = csO_string_object(buffer,fmt.f_size_);
				break;
			case f_bool_:{
				csG_bool v;
				fread(&v, fmt.f_size_, 1, in);
				obj = csO_bool_object(v);
				break;
			}
			default:
				VERIFY(0);
			}
			VERIFY(!csM_const_regin[offset-1]);
			csM_const_regin[offset-1] = obj;
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