#ifndef CS_AST_H
#define CS_AST_H
#include <csglobal.h>
#include <cssymbol.h>
#include <cslist.h>

typedef struct a_dec_ *csA_dec;
typedef csL_list *csA_declist;
typedef struct a_paramt_ *csA_param;
typedef csL_list *csA_paramlist;
typedef csL_list *csA_simpleexpr;;
typedef csL_list *csA_andexpr;
typedef struct a_urelexpr_ *csA_urelexpr;
typedef csL_list *csA_relexpr;
typedef struct a_factor_ *csA_factor;
typedef struct a_mutable_ *csA_mutable;
typedef struct a_immutable_ *csA_immutable;
typedef csL_list *csA_arglist;

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

extern csA_simpleexpr csA_mksimpleexpr();
extern void csA_simpleexpradd(csA_simpleexpr head,csA_andexpr bar);

extern csA_andexpr csA_mkandexpr();
extern void csA_andexpradd(csA_andexpr head,csA_urelexpr bar);

struct a_urelexpr_ {
	csG_bool flags; 
	csA_relexpr rel;
};

extern csA_urelexpr csA_mkurelexpr();
/*
struct sumExpr { CSastTerm term; CSastOp op; CSastSumExpr sum;}; 

struct term {CSastTerm term;CSastOp op;CSastUnExpr uexpr;}; 

struct unaryExpr {CSastOp op;CSastFactor factor;};
*/

struct a_factor_ {
	enum {csA_immut,csA_mut} kind;
	csG_pos pos;
	union {
		csA_immutable immut;
		csA_mutable mut;
	} u;
};
extern csG_pos csA_factorpos(csA_factor foo);
extern csA_factor csA_mkfactor();
extern void csA_setfactormut(csA_factor foo,csA_mutable mut);
extern void csA_setfactorimmut(csA_factor foo,csA_immutable immut);
extern csA_mutable csA_factormut(csA_factor foo);
extern csA_immutable csA_factorimmut(csA_factor foo);


struct a_mutable_ {
	csG_pos pos;
	csS_symbol id; 
	//CSastExpr expr;
};
extern csG_pos csA_mutpos(csA_mutable foo);
extern csA_mutable csA_mkmut();
extern void csA_setmutid(csA_mutable foo,csS_symbol id);
extern csS_symbol csA_mutid(csA_mutable foo);

struct a_immutable_ {
	csG_pos pos;
	enum {
		csA_expr,csA_call,csA_num,csA_char,
		csA_str,csA_bool
	} kind;
	union {
		int val; 	// NUM CHAR true false
		csG_string str;	// STRING
		//CSastExpr expr;
		/* exprList is optional */
		struct {csA_arglist args;csS_symbol id;} call;
	} u;
};
extern csG_pos csA_immutpos(csA_immutable foo);
extern csA_immutable csA_mkimmut();
extern void csA_setimmutstr(csA_immutable foo,csG_string str);
extern void csA_setimmutnum(csA_immutable foo,int num);
extern void csA_setimmutchar(csA_immutable foo,int c);
extern void csA_setimmutbool(csA_immutable foo,csG_bool b);
extern void csA_setimmutcallargs(csA_immutable foo,csA_arglist args);
extern void csA_setimmutcallid(csA_immutable foo,csS_symbol id);
extern csS_symbol csA_immutcallid(csA_immutable foo);
extern csA_arglist csA_immutcallargs(csA_immutable foo);
extern csG_bool csA_immutbool(csA_immutable foo);
extern int csA_immutchar(csA_immutable foo);
extern int csA_immutnum(csA_immutable foo);
extern csG_string csA_immutstr(csA_immutable foo);






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