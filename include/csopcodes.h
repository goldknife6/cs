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
enum csO_opmode {
	iABC, iABx, iAx,iAsBx
};  /* basic instruction format */
#define NUM_OPCODES 15
/*
** R(x) - register
** Cons(x) - constant (in constant regin)
*/
typedef enum {
	//name				args		description					format
	OP_MOVE,		/*	A B			R(A) := R(B)				iABC			*/
	OP_LOADCONST,	/*	A Bx		R(A) := Cons(Bx)			iABx			*/
	OP_ADD,			/*	A B C		R(A) := R(B) + R(C)			iABC			*/
	OP_SUB,			/*	A B C		R(A) := R(B) - R(C)			iABC			*/
	OP_MUL,			/*	A B C		R(A) := R(B) * R(C)			iABC			*/
	OP_DIV,			/*	A B C		R(A) := R(B) / R(C)			iABC			*/
	OP_AND,			/*	A B C		R(A) := R(B) & R(C)			iABC			*/
	OP_OR,			/*	A B C		R(A) := R(B) | R(C)			iABC			*/
	OP_NOT,			/*	A B C		R(A) := not R(B)			iABC			*/

	OP_CONCAT,		/*	A B C		R(A) := R(B) concat R(C)	iABC			*/
	OP_CALL,		/*	A B C		R(A)										*/
	OP_JMP,			/*	sBx			pc+=sBx						iAsBx			*/
	OP_EQ,			/*	A B C		if (R(B) == R(C))			then pc++		*/
	OP_LT,			/*	A B C		if (R(B) <  R(C)) 			then pc++		*/
	OP_LE,			/*	A B C		if (R(B) <= R(C))		 	then pc++		*/
} csO_opcode;

#endif/*!CS_OPCODES_H*/