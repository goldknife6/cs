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
csT_label csT_newlabel(void)
{
	char buf[100];
	static int count;
	csT_temp foo = csU_malloc(sizeof(*foo));
	sprintf(buf,"L%d",count++);
	return csT_namedlabel(csU_strdup(buf));
}
void printTemp(csT_temp h)
{
	VERIFY(h);
	fprintf(debugs, "T%d",h->num);
}
