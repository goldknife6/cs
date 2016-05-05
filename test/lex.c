#include "cslex.h"
#include "csutil.h"



void testlex() 
{
	csL_token tok = csL_gettoken();

	while (tok.kind != csL_ENDFILE) {
		csL_prttoken(tok);
		tok = csL_gettoken();
	}
}