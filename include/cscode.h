#ifndef CS_CODE_H
#define CS_CODE_H
#include "csglobal.h"
#include "cssymbol.h"
#include "csast.h"
#include "csenv.h"

typedef struct c_info_ {

} csC_info;

typedef struct c_address_ csC_address;
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
	} u;
};

typedef struct c_quad_ csC_quad;
struct c_quad_ {
	enum {
		csC_lable,csC_assign,csC_goto,
		csC_iffalse,csC_if,csC_add,csC_call,
		csC_param,csC_minus,csC_multiply,
		csC_divide,csC_sub
	} kind;
	csC_address arg1,arg2,res;
};


struct c_frag_ {
	enum { csC_decfrag , csC_procfrag } kind ;
	union {
		//CSaddrList dec;
		struct {
			//CSaddrList body;
			//CSframe frame;
		} proc ;
	} u ;
};















csC_info c_declist_(csS_table val,csS_table type,csA_declist list);
#endif/*!CS_CODE_H*/