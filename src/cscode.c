#include "cscode.h"
#include "cssymbol.h"
#include "cslist.h"



static csC_info c_info_();
static csC_info c_dec_(csS_table val,csS_table type,csA_dec list);

static csC_info c_info_()
{

}

csC_info c_declist_(csS_table val,csS_table type,csA_declist list)
{
	csC_info inf = c_info_();
	if (!list) return inf;
	csA_dec pos = NULL;
	list_for_each_entry(pos, list, next) {
		VERIFY(pos);
		c_dec_(val,type,pos);
	}
}

static csC_info c_dec_(csS_table vtab,csS_table ttab,csA_dec foo)
{
	VERIFY(foo);
	csC_info inf = c_info_();
	switch (foo->kind) {
	case csA_vardec:{
		csS_symbol tyname = csA_decvartype(foo);
		csS_symbol name = csA_decvarname(foo);
		csE_enventry e = csS_look(ttab, tyname);
		VERIFY(e);
		e = csS_look(vtab, name);
		VERIFY(!e);
		csT_type ty = csE_varty(e);
		//csE_varentry(csT_type type,csF_access access,csS_symbol name);
		csS_insert(vtab, name, e);
		csA_simplelist list = csA_decvarlist(foo);
		if (list) {

		}
		break;
	}
	case csA_fundec:
		break;
	default:
		VERIFY(0);
	}
	return inf;
}