#ifndef CS_CODE_H
#define CS_CODE_H
#include "csglobal.h"
#include "cssymbol.h"
#include "csast.h"
#include "csenv.h"
#include "cslist.h"
#include "csframe.h"
#include <string.h>

typedef csL_list *csC_quadlist;
typedef csL_list *csC_fraglist;

typedef struct c_quad_ *csC_quad;
typedef struct c_frag_ *csC_frag;

typedef struct c_address_ csC_address;

struct c_address_ {
	enum {
		csC_empty,csC_intconst,csC_strconst,
		csC_boolconst,csC_env,caC_lable
	} kind;
	union {
		int ival;
		csG_string str;
		csG_bool bval;
		csE_enventry eval;
		csT_label lab;
	} u;
};

typedef enum {
	csC_load,//load constant,static variable,local variable
	csC_loadaddr,//load address
	csC_stores,//store top to address below top & pop both
	csC_storel,//store top to address below top & pop both
	csC_mark,//mark stack
	csC_add,
	csC_sub,
	csC_mul,
	csC_div,
	csC_eq,
	csC_neq,
	csC_lt,
	csC_lq,
	csC_gt,
	csC_gq,
	csC_fjp,
	csC_tjp,
	csC_ujp,
	csC_lab,
	csC_cup,
	csC_cbip,
	csC_ret,
	csC_or,
	csC_and,
	csC_minus,
	csC_not,
	csC_entry,
	csC_ssp,//sub sp
	csC_prv,//push ret val
} c_quadkind_;

struct c_quad_ {
	c_quadkind_ kind;
	csC_address arg;
	csL_list next;
};

struct c_frag_ {
	enum { 
		csC_procfrag ,csC_strfrag,
		csC_intfrag , csC_boolfrag
	} kind;
	csS_symbol name;
	int offset;
	csL_list next;
	union {
		csG_string strv;
		int intv;
		csG_bool boolv;
		struct {
			csC_quadlist body;
			csF_frame frame;
		} proc;
	} u;
};

extern csC_fraglist c_declist_(csS_table val,csS_table type,csA_declist list);
#endif/*!CS_CODE_H*/