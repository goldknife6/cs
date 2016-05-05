#include "cssymbol.h"
#include "csutil.h"

void s_free(void *key,void *val)
{

}

void testsym()
{
	csS_table tab = csS_empty(s_free,NULL);
	for (int i = 1; i < 111; i++) {
		char buf[100];
		sprintf(buf,"dsfs%dsdf",i);
		csS_symbol s = csS_mksymbol(csU_strdup(buf));
		csS_insert(tab, s, (void*)(long)i);
	}
	for (int i = 1; i < 111; i++) {
		char buf[100];
		sprintf(buf,"dsfs%dsdf",i);
		csS_symbol s = csS_mksymbol(csU_strdup(buf));
		VERIFY(csS_look(tab, s));
	}
	csS_tabfree(tab);
}