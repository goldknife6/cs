#ifndef CS_ENV_H
#define CS_ENV_H
#include "csglobal.h"
#include "cssymbol.h"
#include "cstype.h"
#include "cslist.h"
#include "csframe.h"

typedef struct e_env_ *csE_enventry;

struct e_env_ {
	enum {
		csE_var,csE_fun
	} kind;
	csS_symbol name;
	union {
		struct {
			csT_type type;
			csF_access access;
		} var;
		struct {
			csT_typelist formals;
			csT_type res;
			//csF_frame frame;
		} fun;
	} u;
};

extern csS_table csE_baseval(void);
extern csS_table csE_basetype(void);
extern csE_enventry csE_varentry(csT_type type,csF_access access,csS_symbol name);
extern csE_enventry csE_funentry(csT_typelist formals,csT_type res,csS_symbol name);
#endif/*!CS_ENV_H*/