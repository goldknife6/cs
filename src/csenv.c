#include "csenv.h"
#include "cstype.h"
#include "cslist.h"
#include "cssymbol.h"

struct e_env_ {
	enum {csE_var,csE_fun} kind;
	csS_symbol name;
	union {
		struct {
			csT_type type;
		//	CSaccess access;
		} var;
		struct {
			csL_list formals;
			csT_type res;
		//	CSframe frame;
		} fun;
	} u;
};