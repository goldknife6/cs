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
	enum { f_frame , f_static} kind;
	int offset;
};

struct f_frame_ {
	int offset;
	int framesize;
};

extern csF_frame csF_newframe(); 
extern csF_access csF_alloclocal(csF_frame frame);
extern csF_access csF_allocstatic();
extern csF_frame csF_buildin_frame(int offset);
#endif/*!CS_FRAME_H*/