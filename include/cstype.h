#ifndef CS_TYPES_H
#define CS_TYPES_H
#include "csglobal.h"
#include "cslist.h"
#include "cssymbol.h"

typedef struct t_type_ 		*csT_type;
typedef struct t_field_ 	*csT_field;
typedef struct t_fieldlist_ *csT_fieldlist;
typedef struct t_typelist_  *csT_typelist;

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
	csL_list next;
};

struct t_field_ {
	csS_symbol name; 
	csT_type type;
	csL_list next;
};

struct t_fieldlist_ {
	csL_list head;
};

typedef struct t_typelistentry_ {
	csL_list next;
	csT_type type;
}*t_typelistentry_;

struct t_typelist_ {
	csL_list head;
	int count;
};
extern csT_typelist csT_mktypelist();
extern void csT_typelistadd(csT_typelist head,csT_type type);
//extern csG_bool CStypeEqual(csT_type foo,csT_type bar);
extern csT_type csT_typeint(void);
extern csT_type csT_typebool(void);
extern csT_type csT_typestring(void);
extern csT_type csT_typevoid(void);
#endif/*!CS_TYPES_H*/