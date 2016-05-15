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
	"IFFALSE",
	"RETURN",
	"IF",
	"PARAM",
	NULL	
};

static csG_byte csO_opmodes[NUM_OPCODES] = {

};

csO_code csO_iABC(csO_opcode op,csG_byte a,csG_byte b,csG_byte c)
{
	csO_code cc;
	cc.u.abc.b[0] = op;
	cc.u.abc.b[1] = a;
	cc.u.abc.b[2] = b;
	cc.u.abc.b[3] = c;
	//cc.kind = iABC;
	//printf("%s %d %d %d\n", csO_opnames[op],a,b,c);
	return cc;
}

csO_code csO_iABx(csO_opcode op,csG_byte a,csG_2byte bx)
{
	csO_code cc;
	cc.u.abx.b[0] = op;
	cc.u.abx.b[1] = a;
	cc.u.abx.bx = bx;
	//cc.kind = iABx;
	//printf("%s %d %d\n", csO_opnames[op],a,bx);
	return cc;
}

csO_code csO_iAsBx(csO_opcode op,csG_byte a,csG_2byte bx)
{
	csO_code cc;
	cc.u.abx.b[0] = op;
	cc.u.abx.b[1] = a;
	cc.u.abx.bx = bx;
	//cc.kind = iAsBx;
	//printf("%s %d %d\n", csO_opnames[op],a,bx);
	return cc;
}
csO_opcode csO_iop(csO_code cc)
{
	//if (cc.kind == iABC)
	//	return cc.u.abc.b[0];
	return cc.u.abx.b[0];
}
csG_byte csO_iA(csO_code cc)
{
	return cc.u.abc.b[1];
}
csG_byte csO_iB(csO_code cc)
{
	return cc.u.abc.b[2];
}
csG_2byte csO_iBx(csO_code cc)
{
	return cc.u.abx.bx;
}
csG_byte csO_iC(csO_code cc)
{
	return cc.u.abc.b[3];
}
void csO_printcode(csO_code cc)
{
	//switch (cc.kind) {
	//case iAsBx:
	//case iABx:
		printf("%s %d %d  ", csO_opnames[csO_iop(cc)],csO_iA(cc),csO_iBx(cc));
	//	break;
	//case iABC:
		printf("%d %d %d\n", csO_iA(cc),csO_iB(cc),csO_iC(cc));
	//	break;
	//default:
	//	VERIFY(0);
	//}
	
}
