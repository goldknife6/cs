#include "cstype.h"
#include "cslist.h"
#include "cssymbol.h"

struct t_type_ {
	enum {
		csT_record, csT_int, csT_string,
		csT_array,csT_bool, csT_void
	} kind;
	union {
		csT_fieldlist record;
		csT_type array;
		struct {
			csS_symbol name; 
			csT_type type;
		} name;
	} u;
};

struct t_field_ {
	csS_symbol name; 
	csT_type type;
	csL_list next;
};

struct t_fieldlist_ {
	csL_list head;
};
