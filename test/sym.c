#include "cssymbol.h"
#include "csutil.h"

void s_free(void *key,void *val)
{
	printf("%d\n",(long)val);
}

void testsym()
{
	csS_table tab = csS_empty(s_free,NULL);
	char buf[100];
	csS_symbol s;
	sprintf(buf,"dsfs%dsdf",1);
	s = csS_mksymbol(csU_strdup(buf));
	csS_insert(tab, s, (void*)(long)1);
	VERIFY(csS_look(tab, s) == (void*)1);
	csS_beginscope(tab);
	VERIFY(!csS_look(tab, s));
	csS_insert(tab, s, (void*)(long)2);
	VERIFY(csS_look(tab, s) == (void*)2);
	csS_beginscope(tab);
	VERIFY(!csS_look(tab, s));
	csS_insert(tab, s, (void*)(long)3);
	VERIFY(csS_look(tab, s) == (void*)3);
	csS_endscope(tab);
	csS_endscope(tab);
	csS_tabfree(tab);
}