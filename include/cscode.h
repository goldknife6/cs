#ifndef CS_CODE_H
#define CS_CODE_H
#include "csglobal.h"
#include "cssymbol.h"
#include "csast.h"
#include "csenv.h"
#include "cslist.h"
#include "csframe.h"

typedef struct c_info_ {

} csC_info;



typedef csL_list csC_quadlist;
typedef struct c_quad_ *csC_quad;
typedef struct c_address_ csC_address;
typedef csL_list csC_fraglist;
typedef struct c_frag_ *csC_frag;

extern csC_fraglist fraglist;

struct c_address_ {
	enum {
		csC_empty,csC_intconst,csC_strconst,
		csC_boolconst,csC_env,csC_temp,caC_lable
	} kind;
	union {
		int ival;
		csG_string str;
		csG_bool bval;
		csE_enventry eval;
		csT_temp tmp;
	} u;
};

struct c_quad_ {
	enum {
		csC_lable,csC_assign,csC_goto,
		csC_iffalse,csC_if,csC_add,csC_call,
		csC_param,csC_minus,csC_multiply,
		csC_divide,csC_sub
	} kind;
	csC_address arg1,arg2,res;
	csL_list next;
};

struct c_frag_ {
	csL_list next;
	enum { 
		csC_procfrag ,csC_strfrag,
		csC_intfrag , csC_boolfrag
	} kind;
	union {
		csF_access access;
		struct {
			csG_string strv;
		} strv;
		struct {
			int intv;
		} intv;
		struct {
			csG_bool boolv;
		} boolv;
		struct {
			csC_quadlist body;
			csF_frame frame;
		} proc;
	} u;
};

csC_info c_declist_(csS_table val,csS_table type,csA_declist list);
#endif/*!CS_CODE_H*/