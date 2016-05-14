#include "csframe.h"
#include "cstemp.h"
#include "csutil.h"

csF_frame csF_newframe(csT_label name)
{
	VERIFY(name);
	static int offset = 1;
	csF_frame foo =  csU_malloc(sizeof(*foo));
	foo->name = name;
	foo->offset = offset++;
	return foo;
}

csF_access csF_alloclocal(csF_frame frame)
{
	VERIFY(frame);
	csF_access foo = csU_malloc(sizeof(*foo));
	foo->kind = f_reg;
	foo->u.reg = csT_newtemp();
	return foo;
}

csF_access csF_allocglobal(csT_type type)
{
	VERIFY(type);
	static int offset = 1;
	csF_access foo = csU_malloc(sizeof(*foo));
	switch (type->kind) {
	case csT_int:
		foo->kind = f_static;
		break;
	case csT_string:
		foo->kind = f_static;
		break;
	case csT_bool:
		foo->kind = f_static;
		break;
	default:
		VERIFY(0);
	}
	foo->u.offset = offset++;
	return foo;
}
