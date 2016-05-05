#include "cshash.h"
#include "stdio.h"
/*
csH_tabempty(csH_tabhashfp hfp,csH_tabequalfp efp,csH_tabfreefp ffp,csH_tabshowfp sfp);
void csH_tabinsert(csH_table tab, void *key, void *value);
void *csH_tablook(csH_table tab, void *key);
*/

static void show(void *key, void *value)
{
	fprintf(debugs, "key:%ld value:%ld\n",(long)key,(long)value);
}

void testhash()
{
	csH_table tab1 = csH_tabempty(NULL,NULL,NULL);
	for(long i = 1; i < 110; i++)
		csH_tabinsert(tab1, (void *)i, (void *)i);
	for(long i = 1; i < 110; i++) {
		long res = (long)csH_tablook(tab1, (void *)i);
		VERIFY(res == i);
	}
	csH_tabdump(tab1, show);
	csH_tabfree(tab1);
}