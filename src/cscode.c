#include "cscode.h"

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
	}
}

static csC_info c_dec_(csS_table val,csS_table type,csA_dec list)
{
	
}