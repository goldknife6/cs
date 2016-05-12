#include "csopcodes.h"

static char *csO_opnames[NUM_OPCODES+1] = {
	"MOVE",
	"LOADCONST",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"AND",
	"OR",
	"NOT",
	"CONCAT",
	"CALL",
	"JMP",
	"EQ",
	"LT",
	"LE",
	NULL	
};

static csG_byte csO_opmodes[NUM_OPCODES] = {

};