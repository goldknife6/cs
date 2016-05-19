#ifndef CS_SYMBOL_H
#define CS_SYMBOL_H
#include "csglobal.h"
#include "cshash.h"

typedef struct s_symbol_ *csS_symbol;
typedef struct s_table_ *csS_table;


extern void*		csS_look(csS_table t, csS_symbol sym);
extern csS_table 	csS_empty(csH_tabfreefp fp);
extern void 		csS_insert(csS_table tab, csS_symbol sym, void *value);
extern csG_string 	csS_name(csS_symbol sym);
extern csS_symbol 	csS_mksymbol(csG_string name);
extern void 		csS_tabfree(csS_table t);
extern void 		csS_beginscope(csS_table tab);
extern void 		csS_endscope(csS_table tab);
extern void*		csS_looktop(csS_table tab, csS_symbol sym);
extern csG_bool		csS_compare(csS_symbol sym1, csS_symbol sym2);
#endif/*!CS_SYMBOL_H*/
