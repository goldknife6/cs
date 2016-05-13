#include "csframe.h"
#include "cstemp.h"
#include "csutil.h"

csF_frame csF_newframe(csT_label name)
{
	VERIFY(name);
	csF_frame foo =  csU_malloc(sizeof(*foo));
	foo->name = name;
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
	static int offset = 0;
	csF_access foo = csU_malloc(sizeof(*foo));
	switch (type->kind) {
	case csT_int:
		foo->kind = f_intstatic;
		break;
	case csT_string:
		foo->kind = f_strstatic;
		break;
	case csT_bool:
		foo->kind = f_boolstatic;
		break;
	default:
		VERIFY(0);
	}
	foo->u.offset = offset++;
	return foo;
}
