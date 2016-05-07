#include "csglobal.h"
#include "cstemp.h"

typedef struct f_access_ *csF_access;
struct f_access_ {
	enum { f_frame , f_reg ,f_intstatic,f_strstatic} kind  ;
	union{
		int offset ;
		csT_temp reg ;
	} u ;
};
