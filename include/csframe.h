#ifndef CS_FRAME_H
#define CS_FRAME_H
#include "csglobal.h"
#include "cstemp.h"
#include "csutil.h"
#include "cslist.h"
#include "cstype.h"

typedef struct f_access_ *csF_access;

typedef struct f_frame_ *csF_frame;

struct f_access_ {
	enum { f_frame , f_reg ,f_intstatic,f_strstatic,f_boolstatic} kind;
	union{
		int offset;
		csT_temp reg ;
	} u ;
};


struct f_frame_ {
	csT_label name; 
	int framesize ;
};

extern csF_frame csF_newframe(csT_label name); 
extern csF_access csF_alloclocal(csF_frame frame);
extern csF_access csF_allocglobal(csT_type type);
#endif/*!CS_FRAME_H*/