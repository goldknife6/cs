#include "csframe.h"
#include "cstemp.h"
#include "csutil.h"

csF_access csF_alloclocal(csF_frame frame)
{
	VERIFY(frame);
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_reg;
}

csF_access csF_allocglobalstr()
{
	static int offset = 0;
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_strstatic;
	foo->u.offset = offset++;
	return foo;
}

csF_access csF_allocglobalint()
{
	static int offset = 0;
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_intstatic;
	foo->u.offset = offset++;
	return foo;
}

csF_access csF_allocglobalbool()
{
	static int offset = 0;
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_boolstatic;
	foo->u.offset = offset++;
	return foo;
}