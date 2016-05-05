#include "cstype.h"

struct t_type_ {
	enum {
		csT_record, csT_int, csT_string,
		csT_array,csT_bool, csT_void
	} kind;
	union {
		//CSfieldList record;
		//CStype array;
		//struct {CSsymSymbol sym; CStype type;} name;
	} u;
};