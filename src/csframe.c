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
	static int offset_int = 0;
	static int offset_str = 0;
	static int offset_bool = 0;
	csF_access foo = csU_malloc(sizeof(*foo));
	switch (type->kind) {
	case csT_int:
		foo->kind = f_intstatic;
		foo->u.offset = offset_int++;
		break;
	case csT_string:
		foo->kind = f_strstatic;
		foo->u.offset = offset_str++;
		break;
	case csT_bool:
		foo->kind = f_boolstatic;
		foo->u.offset = offset_bool++;
		break;
	default:
		VERIFY(0);
	}
	return foo;
}
