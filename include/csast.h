#ifndef CS_AST_H
#define CS_AST_H
#include <csglobal.h>
#include <cssymbol.h>
#include <cslist.h>



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
			//CSastParaList para;/*may empty*/
			csS_symbol restype;
			//CSastLocVarDecList locList;/*may empty*/
			//CSastStmtList stmtList;/*may empty*/
		} fundec;
	} u;
};

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

extern csA_dec csA_mkdec(void);
extern csG_pos csA_decpos(csA_dec foo);
extern csS_symbol csA_decvartype(csA_dec foo);
extern csS_symbol csA_decvarname(csA_dec foo);
extern csS_symbol csA_decfunrestype(csA_dec foo);
extern csS_symbol csA_decfunname(csA_dec foo);
extern void csA_setdecvartype(csA_dec foo,csS_symbol type);
extern void csA_setdecvarname(csA_dec foo,csS_symbol name);
extern void csA_setdecfunrestype(csA_dec foo,csS_symbol restype);
extern void csA_setdecfunname(csA_dec foo,csS_symbol name);

typedef csL_list *csA_declist;
extern csA_declist csA_mkdeclist(void);
extern void csA_declistadd(csA_declist foo,csA_dec bar);

#endif/*!CS_AST_H*/