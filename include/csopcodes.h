#ifndef CS_OPCODES_H
#define CS_OPCODES_H
#include "csglobal.h"
/*
* We assume that instructions are unsigned numbers.
*  All instructions have an opcode in the first 8 bits.
*  Instructions can have the following fields:
*	`A' : 8 bits
*	`B' : 8 bits
*	`C' : 8 bits
*	'Ax' : 24 bits ('A', 'B', and 'C' together)
*	`Bx' : 16 bits (`B' and `C' together)
*	`sBx' : signed Bx
*/
typedef enum csO_opmode {
	iABC, iABx, iAx,iAsBx
}csO_opmode ;  /* basic instruction format */
#define NUM_OPCODES 18
/*
** R(x) - register
** Cons(x) - constant (in constant regin)
*/

typedef char csG_byte;
typedef short csG_2byte;
typedef struct {
	//csO_opmode kind;
	union {
		struct {
			csG_byte b[4];
		} abc;
		struct {
			csG_byte b[2];
			csG_2byte bx;
		} abx;
	} u;
} csO_code;

typedef enum {
	//name				args		description					format
	OP_MOVE,		/*	A B			R(A) := R(B)				iABC			0*/
	OP_LOADCONST,	/*	A Bx		R(A) := Cons(Bx)			iABx			1*/
	OP_ADD,			/*	A B C		R(A) := R(B) + R(C)			iABC			2*/
	OP_SUB,			/*	A B C		R(A) := R(B) - R(C)			iABC			3*/
	OP_MUL,			/*	A B C		R(A) := R(B) * R(C)			iABC			4*/
	OP_DIV,			/*	A B C		R(A) := R(B) / R(C)			iABC			5*/
	OP_AND,			/*	A B C		R(A) := R(B) & R(C)			iABC			6*/
	OP_OR,			/*	A B C		R(A) := R(B) | R(C)			iABC			7*/
	OP_NOT,			/*	A B C		R(A) := not R(B)			iABC			8*/

	OP_CONCAT,		/*	A B C		R(A) := R(B) concat R(C)	iABC			9*/
	OP_CALL,		/*	A B C		R(A)										10*/
	OP_JMP,			/*	sBx			pc+=sBx						iAsBx			11*/
	OP_EQ,			/*	A B C		if (R(B) == R(C))			then pc++		12*/
	OP_LT,			/*	A B C		if (R(B) <  R(C)) 			then pc++		13*/
	OP_LE,			/*	A B C		if (R(B) <= R(C))		 	then pc++		14*/
	OP_IFFALSE,     /*	A sBx		if(not A) pc+=sBx			iAsBx			15*/
	OP_RETURN,      /*	A 										iABC			16*/
	OP_IF,     		/*	A sBx		if(not A) pc+=sBx			iAsBx			17*/
} csO_opcode;


extern csO_code csO_iABC(csO_opcode op,csG_byte a,csG_byte b,csG_byte c);
extern csO_code csO_iABx(csO_opcode op,csG_byte a,csG_2byte bx);
extern csO_code csO_iAsBx(csO_opcode op,csG_byte a,csG_2byte bx);
extern csO_opcode csO_iop(csO_code op);
extern csG_byte csO_iA(csO_code op);
extern csG_byte csO_iB(csO_code op);
extern csG_byte csO_iC(csO_code op);
extern csG_2byte csO_iBx(csO_code cc);
extern void csO_printcode(csO_code cc);
#endif/*!CS_OPCODES_H*/