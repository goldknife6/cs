#include "csmemory.h"
#include "csformat.h"

static void csF_printfmt(csF_format fmt);

void csM_load_bytecode(FILE *in)
{
	VERIFY(in);
	csF_format fmt;
	fread(&fmt, sizeof(csF_format), 1, in);
	csF_printfmt(fmt);
}

/*
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
*/
static void csF_printfmt(csF_format fmt)
{
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
		break;
	default:
		VERIFY(0);
	}
}

