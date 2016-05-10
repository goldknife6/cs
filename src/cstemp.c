#include "cstemp.h"
#include "cslist.h"
#include "csutil.h"

struct t_temp_ {
	int num;
};

struct t_templist_ {
	csL_list head;
};

csT_temp csT_newtemp(void)
{
	static int count;
	csT_temp foo = csU_malloc(sizeof(*foo));
	foo->num = count++;
	return foo;
}

csT_label csT_namedlabel(csG_string name)
{
	VERIFY(name);
	csS_symbol s = csS_mksymbol(name);
	return s;
}

void printTemp(csT_temp h)
{
	VERIFY(h);
	fprintf(debugs, "T%d",h->num);
}
