#include "cssymbol.h"
#include "csutil.h"
#include "cshash.h"
#include "cslist.h"
struct s_symbol_ {
	csG_string name;
};

typedef struct s_table_ele {
	csH_table tab;
	csL_list next;	
}s_table_ele;

struct s_table_ {
	csH_table top;
	csL_list head;
	csH_tabfreefp fp;
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
	VERIFY(tab);VERIFY(tab->top);
	VERIFY(sym);VERIFY(value);
	csH_tabinsert(tab->top, sym, value);
}

csS_table csS_empty(csH_tabfreefp fp,csS_table top)
{
	VERIFY(fp);
	csS_table tab  = csU_malloc(sizeof(*tab));
	tab->top = csH_tabempty(NULL,NULL,fp);
	INIT_LIST_HEAD(&tab->head);
	tab->fp = fp;
	return tab;
}

void *csS_look(csS_table tab, csS_symbol sym)
{
	VERIFY(tab);
	VERIFY(sym);
	VERIFY(tab->top);
	return csH_tablook(tab->top, sym);
}

void csS_tabfree(csS_table tab)
{
	VERIFY(tab);
	VERIFY(tab->top);
	VERIFY(list_empty(&tab->head));
	csH_tabfree(tab->top);
	csU_free(tab);
}

void csS_beginscope(csS_table tab)
{
	VERIFY(tab);
	VERIFY(tab->top);
	VERIFY(tab->fp);
	s_table_ele *ele  = csU_malloc(sizeof(*ele));
	ele->tab = tab->top;
	INIT_LIST_HEAD(&ele->next);
	tab->top = csH_tabempty(NULL,NULL,tab->fp);
	list_add(&ele->next,&tab->head);
}

void csS_endscope(csS_table tab)
{
	VERIFY(tab);
	VERIFY(tab->top);
	VERIFY(!list_empty(&tab->head));
	s_table_ele *ele = NULL;
	ele = list_entry(tab->head.next, s_table_ele, next);
	VERIFY(ele);
	VERIFY(ele->tab);
	csH_tabfree(tab->top);
	list_del(&ele->next);
	tab->top = ele->tab;
	csU_free(ele);
} 