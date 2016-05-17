#include "csframe.h"
#include "cstemp.h"
#include "csutil.h"

csF_frame csF_newframe()
{
	static int offset = 1;
	csF_frame foo =  csU_malloc(sizeof(*foo));
	foo->offset = offset++;
	return foo;
}

csF_access csF_alloclocal(csF_frame frame)
{
	VERIFY(frame);
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_frame;
	foo->offset = ++frame->framesize;
	return foo;
}

csF_access csF_allocstatic()
{
	static int offset = 1;
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->offset = offset++;
	foo->kind = f_static;
	return foo;
}
