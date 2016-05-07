#include "cscode.h"
#include "csparser.h"
#include "csenv.h"



void testcode(void)
{
	csA_declist list = parser();
	csS_table val = csE_baseval();
	csS_table type = csE_basetype();
	c_declist_(val,type,list);
}