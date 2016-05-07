#ifndef CS_FRAME_H
#define CS_FRAME_H
#include "csglobal.h"
#include "cstemp.h"
#include "csutil.h"

typedef struct f_access_ *csF_access;
struct f_access_ {
	enum { f_frame , f_reg ,f_intstatic,f_strstatic,f_boolstatic} kind;
	union{
		int offset ;
		csT_temp reg ;
	} u ;
};

typedef struct f_frame_ *csF_frame;
struct f_frame_ {
	csT_label name; 
	int framesize ;
};

extern csF_access csF_alloclocal(csF_frame frame);
extern csF_access csF_allocglobalstr();
extern csF_access csF_allocglobalint();
extern csF_access csF_allocglobalbool();
#endif/*!CS_FRAME_H*/