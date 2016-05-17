#ifndef CS_OPCODES_H
#define CS_OPCODES_H
#include "csglobal.h"
/*
* We assume that instructions are unsigned numbers.
*  All instructions have an opcode in the first 8 bits.
*  Instructions can only have the following fields:
*	`A' : 8 bits
*	`B' : 16 bits
*	`Bx' : signed 24 bits
*/
#define NUM_OPCODES 27
/*
** R(x) - register
** Cons(x) - constant (in constant regin)
*/

typedef char csO_byte;
typedef short csO_2byte;
typedef int csO_4byte;

typedef struct {
	csO_byte opkind;
	csO_4byte d;
} csO_code;

typedef enum {
	//name							description					
	OP_LOADC,/*						load constant						*/
	OP_LOADS,/*						static variable						*/
	OP_LOADL,/*						local variable						*/
	OP_LOADA,/*					load static variable address		*/
	OP_STORES,/*store top to address below top & pop both*/
	OP_STOREL,/*store top to address below top & pop both*/
	OP_MARK,/*mark stack*/
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_EQ,
	OP_NEQ,
	OP_LT,
	OP_LQ,
	OP_GT,
	OP_GQ,
	OP_FJP,/*if false,ip += A*/
	OP_TJP,/*if true,ip += A*/
	OP_UJP,/*ip += A*/
	OP_CUP,/*call user function*/
	OP_CBP,/*call build in function*/
	OP_RET,
	OP_OR,
	OP_AND,
	OP_MINUS,
	OP_NOT,
} csO_opcode;
extern void csO_printcode(csO_code code);
extern csO_byte csO_codeop(csO_code code);
extern csO_code csO_mkcode(csO_byte kind,csO_4byte i);
#endif/*!CS_OPCODES_H*/