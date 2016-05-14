#include "csformat.h"

csF_format csF_static(csF_fmtvalkind kind,int size,int offset)
{
	csF_format foo;
	foo.f_kind_ = f_static_;
	foo.u.f_valkind_ = kind;
	foo.f_size_ = size;
	foo.f_offset_ = offset;
	return foo;
}

csF_format csF_const(csF_fmtvalkind kind,int size,int offset)
{
	csF_format foo;
	foo.f_kind_ = f_const_;
	foo.u.f_valkind_ = kind;
	foo.f_size_ = size;
	foo.f_offset_ = offset;
	return foo;
}

csF_format csF_proc(int size,int offset)
{
	csF_format foo;
	foo.f_kind_ = f_prco_;
	foo.u.f_main_ = 0;
	foo.f_size_ = size;
	foo.f_offset_ = offset;
	return foo;
}
