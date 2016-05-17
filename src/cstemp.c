#include "cstemp.h"
#include "cslist.h"
#include "csutil.h"

struct t_lable_ {
	int num;
};

csT_label csT_newlabel(void)
{
	static int count;
	csT_label foo = csU_malloc(sizeof(*foo));
	foo->num = count++;
	return foo;
}

void csT_plabel(csT_label lab)
{
	VERIFY(lab);
	fprintf(debugs, "L%d", lab->num);
}