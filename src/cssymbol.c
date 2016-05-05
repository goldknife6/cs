#include "cssymbol.h"
#include "csutil.h"
#include "cshash.h"
struct s_symbol_ {
	csG_string name;
};

struct s_table_ {
	csH_table tab;
	csS_table top;
};

static csS_symbol symbol(csG_string name) 
{
	VERIFY(name);
	csS_symbol s = csU_malloc(sizeof(*s));
	s->name = name;
	return s;
}

static csH_table table = NULL;
csS_symbol csS_mksymbol(csG_string name) 
{
	VERIFY(name);
	if (!table)
		table = csH_tabempty(csU_strhash,csU_strequal,NULL);
	csS_symbol s = (csS_symbol)csH_tablook(table, name);	
	if (!s) {
		s = symbol(name);
		csH_tabinsert(table, name, s);
	}
	return s;
}

csG_string csS_name(csS_symbol sym)
{
	VERIFY(sym);
	return sym->name;
}

void csS_insert(csS_table tab, csS_symbol sym, void *value) 
{
	VERIFY(tab);VERIFY(tab->tab);
	VERIFY(sym);VERIFY(value);
	csH_tabinsert(tab->tab, sym, value);
}

csS_table csS_empty(csH_tabfreefp fp,csS_table top)
{
	VERIFY(fp);
	csS_table tab  = csU_malloc(sizeof(*tab));
	tab->tab = csH_tabempty(NULL,NULL,fp);
	tab->top = top;
	return tab;
}

void *csS_look(csS_table t, csS_symbol sym)
{
	VERIFY(t);VERIFY(sym);
	VERIFY(t->tab);
	return csH_tablook(t->tab, sym);
}

void csS_tabfree(csS_table t)
{
	VERIFY(t);
	VERIFY(t->tab);
	return csH_tabfree(t->tab);
}