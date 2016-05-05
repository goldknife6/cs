#include "csenv.h"

struct e_env_ {
	enum {csE_var,csE_fun} kind;
	csS_symbol name;
	union {
		//struct {csT_type type;CSaccess access;} var;
		//struct {CStypeList formals;CStype res;CSframe frame;} fun;
	} u;
};