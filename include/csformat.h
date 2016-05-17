#ifndef CS_FORMAT_H
#define CS_FORMAT_H
#include "csglobal.h"

typedef enum {
	f_static_,f_prco_,f_const_
} csF_fmtkind;

typedef enum {
	f_int_,f_str_,f_bool_
} csF_fmtvalkind;

typedef struct {
	csG_int32 f_kind_;
	union {
		csG_int32 f_main_;
		csG_int32 f_valkind_;
	} u;
	csG_int32 f_size_;//the size of data,except header
	csG_int32 f_offset_;
} csF_format;

#define FORMATID 0x22768954

typedef struct {
	csG_int32 id;
	csG_int32 staticsize;
	csG_int32 constsize;
	csG_int32 procsize;
} csF_fmtheader;

static void csF_printfmt(csF_format fmt)
{
	fprintf(stdout, "%04x%04x%04x%04x\n",fmt.f_kind_,fmt.u.f_valkind_,fmt.f_size_,fmt.f_offset_);
	switch (fmt.f_kind_) {
	case f_static_:
		fprintf(stdout, "static:");
		switch (fmt.u.f_valkind_) {
		case f_int_:
			fprintf(stdout, "int:");
			break;
		case f_str_:
			fprintf(stdout, "string:");
			break;
		case f_bool_:
			fprintf(stdout, "bool:");
			break;
		default:
			VERIFY(0);
		}
		break;
	case f_prco_:
		break;
	case f_const_:
		fprintf(stdout, "const:");
		switch (fmt.u.f_valkind_) {
		case f_int_:
			fprintf(stdout, "int:");
			break;
		case f_str_:
			fprintf(stdout, "string:");
			break;
		case f_bool_:
			fprintf(stdout, "bool:");
			break;
		default:
			VERIFY(0);
		}
		break;
	default:
		VERIFY(0);
	}
	fprintf(stdout, "\n");
}


extern csF_format csF_static(csF_fmtvalkind kind,int size,int offset);
extern csF_format csF_const(csF_fmtvalkind kind,int size,int offset);
extern csF_format csF_proc(int size,int offset);
extern csF_fmtheader csF_header(int staticsize,int constsize,int procsize);
#endif/*!CS_FORMAT_H*/