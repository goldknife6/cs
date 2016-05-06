#ifndef CS_AST_H
#define CS_AST_H
#include <csglobal.h>
#include <cssymbol.h>
#include <cslist.h>

typedef struct a_paramt_ *csA_param;
typedef csL_list *csA_paramlist;

typedef struct a_dec_ *csA_dec;
struct a_dec_ {
	enum {
		csA_vardec,csA_fundec
	} kind;
	csG_pos pos;
	csL_list next;
	union {
		struct {
			csS_symbol type;
			csS_symbol name;
			//CSastSmpExpr smp;/*smp may null*/
		} vardec;
		struct {
			csS_symbol name;
			csA_paramlist list;/*may empty*/
			csS_symbol restype;
			//CSastLocVarDecList locList;/*may empty*/
			//CSastStmtList stmtList;/*may empty*/
		} fundec;
	} u;
};
extern csA_dec csA_mkdec(void);
extern csG_pos csA_decpos(csA_dec foo);
extern csS_symbol csA_decvartype(csA_dec foo);
extern csS_symbol csA_decvarname(csA_dec foo);
extern csS_symbol csA_decfunrestype(csA_dec foo);
extern csS_symbol csA_decfunname(csA_dec foo);
extern csA_paramlist csA_decfunparamlist(csA_dec foo);
extern void csA_setdecvartype(csA_dec foo,csS_symbol type);
extern void csA_setdecvarname(csA_dec foo,csS_symbol name);
extern void csA_setdecfunrestype(csA_dec foo,csS_symbol restype);
extern void csA_setdecfunname(csA_dec foo,csS_symbol name);
extern void csA_setdecfunparamlist(csA_dec foo,csA_paramlist list);

typedef csL_list *csA_declist;
extern csA_declist csA_mkdeclist(void);
extern void csA_declistadd(csA_declist foo,csA_dec bar);


struct a_paramt_ {
	csS_symbol type;
	csS_symbol name;
	csL_list next;
	csG_pos pos;
};
extern csA_param csA_mkparam();
extern csS_symbol csA_paramtype(csA_param foo);
extern csS_symbol csA_paramname(csA_param foo);
extern void csA_setparamtype(csA_param foo,csS_symbol type);
extern void csA_setparamname(csA_param foo,csS_symbol name);

extern csA_paramlist csA_mkparamlist(void);
extern void csA_paramlistadd(csA_paramlist foo,csA_param bar);
































typedef struct a_stmt_ *csA_stmt;
typedef enum {
		csA_exprstmt ,csA_compoundstmt,csA_ifstmt,
		csA_whilestmt,csA_forstmt,csA_returnstmt,csA_breakstmt
} csA_stmtkind;





struct a_stmt_ {
	csA_stmtkind kind;
	csG_pos pos;
};

struct a_locvardec_ {
	csG_pos pos;
	csS_symbol typeid;
	csS_symbol id;
	//CSastSmpExpr smp;/*smp may null*/
};





#endif/*!CS_AST_H*/