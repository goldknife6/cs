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


extern csF_format csF_static(csF_fmtvalkind kind,int size,int offset);
extern csF_format csF_const(csF_fmtvalkind kind,int size,int offset);
extern csF_format csF_proc(int size,int offset);
#endif/*!CS_FORMAT_H*/