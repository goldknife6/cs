#include "csopcodes.h"

static char *csO_opnames[NUM_OPCODES+1] = {
	"LOADC",
	"LOADS",
	"LOADL",
	"LOADA",
	"STORES",
	"STOREL",
	"MARK",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"EQ",
	"NEQ",
	"LT",
	"LQ",
	"GT",
	"GQ",
	"FJP",
	"TJP",
	"UJP",
	"CUP",
	"CBP",
	"RET",
	"OR",
	"AND",
	"MINUS",
	"NOT",
	"SSP",
	"PRV",
	"LOADN",
	NULL	
};

void csO_printcode(csO_code code)
{
	fprintf(stderr, "%s %d\n", csO_opnames[csO_codeop(code)],code.d);
}

csO_byte csO_codeop(csO_code code)
{
	return code.opkind;
}

csO_4byte csO_codeval(csO_code code)
{
	return code.d;
}

csO_code csO_mkcode(csO_byte kind,csO_4byte i)
{
	csO_code code;
	code.opkind = kind;
	code.d = i;
	return code;
}

